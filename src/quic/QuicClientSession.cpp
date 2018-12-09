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

#ifndef WIN32
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#endif

#include <net/tools/quic/relay/quic_raw_session.h>
#include <net/quic/quartc/quartc_stream.h>
#include <base/threading/thread_task_runner_handle.h>

#include <aeres/Log.h>
#include <aeres/Util.h>
#include <aeres/ConnectionFactory.h>
#include <aeres/HandshakeInstruction.h>
#include <aeres/BufferedInputStream.h>
#include <aeres/quic/QuicConnection.h>
#include <aeres/client/QuicClientSession.h>

namespace aeres
{
  namespace client
  {
    QuicClientSession::QuicClientSession(ConnectionFactory * factory, rule::RuleConfig * ruleConfig)
      : factory(factory)
      , weakFactory(this)
      , ruleConfig(ruleConfig)
    {
      fail_if(!factory);
      fail_if(!ruleConfig);

      Log::Debug("QuicClientSession::QuicClientSession: this=%p", this);

      this->taskRunner = base::ThreadTaskRunnerHandle::Get().get();
    }


    QuicClientSession::~QuicClientSession()
    {
      Log::Debug("QuicClientSession::~QuicClientSession: this=%p", this);
      this->Close();

      if (this->connection)
      {
        this->connection->SetRecvCallback(nullptr);
      }

      if (this->local)
      {
        this->local->SetRecvCallback(nullptr);
      }
    }


    bool QuicClientSession::Initialize(net::QuicRawSession * session, net::QuartcStream * stream)
    {
      assert(session && stream);

      Log::Debug("QuicClientSession::Initialize: this=%p session=%p stream=%p", this, session, stream);

      this->connection = std::static_pointer_cast<Connection>(std::make_shared<quic::QuicConnection>(session, stream));
      
      return this->connection->SetRecvCallback(
        [this](Connection * conn, const void * data, size_t len)
        {
          assert(conn == this->connection.get());

          if (!data || len == 0)
          {
            this->CloseRemote(this->connection, this->local);
          }
          else if (!this->connection->IsClosed())
          {
            if (!this->local)
            {
              BufferedInputStream stream(static_cast<const uint8_t *>(data), len);
              HandshakeInstruction instr;
              if (!instr.Deserialize(stream))
              {
                Log::Debug("QuicClientSesion::RecvCallback - Failed to deserialize handshake stream.");
                this->Close();
                return;
              }

              Log::Debug("QuicClientSesion::RecvCallback - Received handshake message: hostname=%s port=%u", instr.HostName().c_str(), instr.Port());

              this->OnHandshake(instr.HostName(), instr.Port(), instr.GetProtocol() == HandshakeInstruction::Protocol::TCP);
            }
            else
            {
              this->OnQuicReceived(data, len);
            }
          }
        }
      );
    }


    void QuicClientSession::Close()
    {
      // This function should only be called in QUIC thread.
      this->CloseLocal(this->local, this->connection);
      this->CloseRemote(this->connection, this->local);
    }


    void QuicClientSession::CloseRemote(std::shared_ptr<Connection> connectionPtr,
                                        std::shared_ptr<Connection> localPtr)
    {
      return_if(this->closeRemote);

      Log::Debug("Closing remote from QUIC client session: session=%p remote=%p", this, connectionPtr.get());

      this->closeRemote = true;

      if (connectionPtr && !connectionPtr->IsClosed())
      {
        connectionPtr->SetRecvCallback(nullptr);
        connectionPtr->Close();
      }

      if (localPtr && !localPtr->IsClosed())
      {
        localPtr->Shutdown();
      }

      if (this->onClose && this->closeLocal)
      {
        this->onClose(this);
      }
    }


    void QuicClientSession::CloseLocal(std::shared_ptr<Connection> localPtr,
                                       std::shared_ptr<Connection> remotePtr)
    {
      return_if(this->closeLocal);

      Log::Debug("Closing local from QUIC client session: session=%p local=%p", this, localPtr.get());

      this->closeLocal = true;

      if (localPtr && !localPtr->IsClosed())
      {
        localPtr->Close();
        localPtr->SetRecvCallback(nullptr);
      }

      if (remotePtr && !remotePtr->IsClosed())
      {
        remotePtr->Shutdown();
      }

      if (this->onClose && this->closeRemote)
      {
        this->onClose(this);
      }
    }


    void QuicClientSession::OnQuicReceived(const void * data, size_t len)
    {
      Log::Debug("QuicClientSession::OnQuicReceived: len=%llu", (long long unsigned)len);

      if (!this->local || this->local->IsClosed())
      {
        Log::Warning("QUIC data received without local connection.");
        return;
      }

      Buffer buf;
      buf.Resize(len);
      memcpy(buf.Buf(), data, len);

      if (!this->local->Send(std::move(buf)))
      {
        Log::Warning("Failed to forward QUIC data to local connection.");
      }
  }


    void QuicClientSession::OnLocalData(std::shared_ptr<Connection> connectionPtr,
                                        std::shared_ptr<Connection> localPtr,
                                        const void * data,
                                        size_t len)
    {
      Log::Debug("QuicClientSession::OnLocalData: len=%llu", (long long unsigned)len);

      if (!connectionPtr || connectionPtr->IsClosed())
      {
        Log::Warning("Local connection data received without QUIC connection");
        return;
      }

      Buffer buf;
      buf.Resize(len);
      memcpy(buf.Buf(), data, len);

      if (!connectionPtr->Send(std::move(buf)))
      {
        Log::Warning("Failed to forward local data to QUIC connection");
      }
    }


    static bool resolveHost(const std::string & host, uint8_t ip[4])
    {
#if __linux__
      // gethostbyname is not reentrant on linux, use gethostbyname_r instead
      char buf[BUFSIZ];
      struct hostent hostinfo = {0};
      struct hostent * ptr = nullptr;
      int err = 0;

      if (gethostbyname_r(host.c_str(), &hostinfo, buf, sizeof(buf), &ptr, &err) == 0 &&
          hostinfo.h_addrtype == AF_INET &&
          hostinfo.h_addr_list &&
          hostinfo.h_addr_list[0])
      {
        memcpy(ip, hostinfo.h_addr_list[0], 4);
        return true;
      }
#else
      struct hostent * ptr = gethostbyname(host.c_str());
      if (ptr && ptr->h_addrtype == AF_INET && ptr->h_addr_list && ptr->h_addr_list[0])
      {
        memcpy(ip, hostinfo.h_addr_list[0], 4);
        return true;
      }
#endif

      Log::Error("Failed to resolve hostname %s.", host.c_str());
      return false;
    }



    void QuicClientSession::OnHandshake(const std::string & hostname, uint16_t port, bool tcp)
    {
      if (!tcp)
      {
        Log::Warning("QuicClientSession::OnHandshake: UDP is not implemented (hostname=%s port=%u)", hostname.c_str(), port);
        return;
      }

      bool resolved = false;
      rule::RuleBase::Host outputHost;
      for (const auto & rule : this->ruleConfig->Rules())
      {
        rule::RuleBase::Host input;
        input.hostname = rule::RuleHostName(hostname);
        input.port = port;

        outputHost = rule::RuleBase::Host();
        if (rule->Resolve(input, tcp, outputHost))
        {
          resolved = true;
          break;
        }
      }

      if (!resolved)
      {
        Log::Verbose("QuicClientSession::OnHandshake: no tunnel rule can match the incoming request: hostname=%s port=%u", hostname.c_str(), port);
        return;
      }

      std::string targetHost = outputHost.hostname.ToString();
      uint16_t targetPort = outputHost.port;
      Log::Verbose("QuicClientSession::OnHandshake: connecting to %s:%u by request of %s:%u", targetHost.c_str(), targetPort, hostname.c_str(), port);

      uint8_t ip[4];
      return_if(!resolveHost(targetHost, ip));

      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(targetPort);
      addr.sin_addr.s_addr = *((uint32_t *)ip);

      this->local = this->factory->CreateSocketConnection((struct sockaddr *)&addr, sizeof(addr), false);
      if (!this->local)
      {
        Log::Warning("QuicClientSession: failed to create socket connection to %u.%u.%u.%u:%u", ip[0], ip[1], ip[2], ip[3], port);
        if (!this->taskRunner ||
            !this->taskRunner->PostTask(base::Bind(&QuicClientSession::Close, this->weakFactory.GetWeakPtr())))
        {
          Log::Warning("QuicClientSession: failed to clean up failing session. this=%p", this);
        }
        return; 
      }

      std::weak_ptr<Connection> connectionPtr = this->connection;
      std::weak_ptr<Connection> localPtr = this->local;

      this->local->SetRecvCallback(
        [this, connectionPtr, localPtr](Connection * conn, const void * data, size_t len)
        {
          auto connection = connectionPtr.lock();
          auto local = localPtr.lock();

          Log::Debug("QuicClientSession: local received: this=%p local=%p conn=%p", this, local.get(), connection.get());

          return_if (conn != local.get() || !local);

          if (data && len > 0)
          {
            this->OnLocalData(connection, local, data, len);
          }
          else if ((!local->IsClosed()) && (
                   !this->taskRunner ||
                   !this->taskRunner->PostTask(base::Bind(&QuicClientSession::CloseLocal,
                                                          this->weakFactory.GetWeakPtr(),
                                                          local,
                                                          connection))))
          {
            Log::Warning("QuicClientSession: unable to request QUIC thread to close local connection. this=%p", this);
          }
        }
      );
    }
  }
}
