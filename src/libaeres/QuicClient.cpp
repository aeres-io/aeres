/*
  MIT License

  Copyright (c) 2018 aeres.io

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <assert.h>
#include <functional>
#include <thread>
#include <chrono>
#include <limits>

#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>

#include <base/at_exit.h>
#include <base/message_loop/message_loop.h>
#include <net/base/ip_address.h>
#include <net/quic/core/quic_server_id.h>
#include <net/quic/core/quic_versions.h>
#include <net/tools/quic/relay/quic_raw_client.h>
#include <net/quic/platform/api/quic_flags.h>

#include <aeres/quic/FakeProofVerifier.h>
#include <aeres/quic/QuartcSessionDelegate.h>
#include <aeres/quic/QuartcStreamDelegate.h>
#include <aeres/Util.h>
#include <aeres/Log.h>
#include <aeres/client/QuicClient.h>

namespace aeres
{
  namespace client
  {
    QuicClient::QuicClient(std::string id, std::string host, uint16_t port)
      : id(std::move(id))
      , host(std::move(host))
      , port(port)
    {
      assert(!this->id.empty());
      assert(!this->host.empty());
      assert(port > 0);

      this->factory = std::make_unique<ConnectionFactory>();

      using namespace net;
      SetQuicFlag(& FLAGS_quic_buffered_data_threshold, std::numeric_limits<uint32_t>::max());
    }


    QuicClient::~QuicClient()
    {
      this->Stop();
    }


    bool QuicClient::Start()
    {
      return_false_if(this->active);

      this->active = true;
      this->thread = std::thread(std::bind(&QuicClient::ThreadProc, this));
      return true;
    }


    void QuicClient::Stop()
    {
      this->active = false;

      this->WaitForExit();
    }


    void QuicClient::WaitForExit()
    {
      if (this->thread.joinable())
      {
        this->thread.join();
      }
    }


    bool resolveHost(const std::string & host, ::net::IPAddress & addr)
    {
      char buf[BUFSIZ];
      struct hostent hostinfo = {0};
      struct hostent * ptr = nullptr;
      int err = 0;

      if (gethostbyname_r(host.c_str(), &hostinfo, buf, sizeof(buf), &ptr, &err) == 0 &&
          hostinfo.h_addrtype == AF_INET &&
          hostinfo.h_addr_list &&
          hostinfo.h_addr_list[0])
      {
        addr = ::net::IPAddress(reinterpret_cast<const uint8_t *>(hostinfo.h_addr_list[0]), 4);
        return true;
      }

      Log::Error("Failed to resolve hostname %s.", host.c_str());
      return false;
    }


    void QuicClient::ThreadProc()
    {
      base::AtExitManager exitManager;
      base::MessageLoopForIO messageLoop;

      Log::Debug("Connecting to QUIC server.");

      ::net::IPAddress addr;
      return_if(!resolveHost(this->host, addr));

      ::net::QuicServerId serverId(this->host, this->port, ::net::PRIVACY_MODE_DISABLED);
      auto versions = ::net::AllSupportedVersions();

      std::unique_ptr<::net::ProofVerifier> verifier;
      verifier.reset(new quic::FakeProofVerifier());

      ::net::QuicRawClient client(::net::IPEndPoint(addr, this->port),
                                  serverId,
                                  versions,
                                  std::move(verifier));

      client.set_initial_max_packet_length(::net::kDefaultMaxPacketSize);
      if (!client.Initialize())
      {
        Log::Critical("Failed to initialize QUIC client.");
        return;
      }

      auto sessionDelegate = std::make_unique<quic::QuartcSessionDelegate>();
      sessionDelegate->SetIncomingStreamCallback([this, &client](::net::QuartcStreamInterface * s) {
        Log::Verbose("New incoming QUIC stream connected: s=%p", s);

        auto session = std::make_unique<QuicClientSession>(this->factory.get());
        if (!session->Initialize(static_cast<::net::QuicRawSession *>(client.session()),
                                 static_cast<::net::QuartcStream *>(s)))
        {
          Log::Error("Failed to initialize QUIC client session.");
          return;
        }

        session->SetCloseCallback([this](QuicClientSession * session) {
          std::unique_lock<std::mutex> lock(this->sessionMutex);
          auto itr = this->sessions.find(session);
          if (itr != this->sessions.end())
          {
            // Grab a reference first since this will destroy the callback itself which is running
            auto clientSession = std::move(itr->second);
            this->sessions.erase(itr);
            clientSession->SetCloseCallback(nullptr);
          }
        });

        std::unique_lock<std::mutex> lock(this->sessionMutex);
        this->sessions[session.get()] = std::move(session);
      });

      client.ResetSessionDelegate(sessionDelegate.release());

      if (!client.Connect())
      {
        Log::Critical("Failed to connect to QUIC server. (err=%d)", client.session()->error());
        return;
      }

      Log::Debug("Connected to QUIC server. Starting handshake.");

      auto stream = client.CreateClientStream();
      if (!stream)
      {
        Log::Critical("Failed to create client stream.");
      }

      client.SendData(stream, this->id.c_str(), this->id.size(), false);

      Log::Information("QUIC client initialization completed.");

      while (this->active && client.connected())
      {
        client.WaitForEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }

      Log::Information("Quic client exited.");
    }
  }
}
