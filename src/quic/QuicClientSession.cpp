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
#include <netinet/in.h>
#endif

#include <net/tools/quic/relay/quic_raw_session.h>
#include <net/quic/quartc/quartc_stream.h>
#include <base/threading/thread_task_runner_handle.h>

#include <aeres/Log.h>
#include <aeres/Util.h>
#include <aeres/ConnectionFactory.h>
#include <aeres/quic/QuicConnection.h>
#include <aeres/client/QuicClientSession.h>

namespace aeres
{
  namespace client
  {
    QuicClientSession::QuicClientSession(ConnectionFactory * factory)
      : factory(factory)
      , weakFactory(this)
    {
      assert(factory);

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
              if (len < sizeof(uint16_t))
              {
                this->Close();
                return;
              }

              uint16_t port;
              memcpy(&port, data, sizeof(uint16_t));
              port = ntohs(port);
              this->OnHandshake(port);
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


    void QuicClientSession::OnHandshake(uint16_t port)
    {
      const uint8_t ip[4] = { 127, 0, 0, 1 };

      struct sockaddr_in addr;
      addr.sin_family = AF_INET;
      addr.sin_port = htons(port);
      addr.sin_addr.s_addr = *((uint32_t *)ip);

      Log::Verbose("Connecting to localhost:%u.", port);

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
