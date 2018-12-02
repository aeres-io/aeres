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


#include <aeres/PlatformUtil.h>
#include <aeres/Util.h>
#include <aeres/Log.h>

#include "SocketDispatcherWin.h"
#include <MSWSock.h>

namespace aeres
{
  SocketDispatcherImpl::~SocketDispatcherImpl()
  {
    this->active = false;

    if (this->iocp)
    {
      unused_result(this->NotifyMessage());
    }

    if (this->thread.joinable())
    {
      this->thread.join();
    }

    if (this->iocp)
    {
      CloseHandle(this->iocp);
      this->iocp = NULL;
    }
  }


  bool SocketDispatcherImpl::Initialize()
  {
    this->iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
    return_false_if(this->iocp == NULL);

    ZeroMemory(&this->messageOverlapped, sizeof(this->messageOverlapped));

    this->thread = std::thread(std::bind(&SocketDispatcherImpl::ThreadProc, this));

    return false;
  }


  bool SocketDispatcherImpl::Register(SocketConnectionPtr connection, Listener onReceive, bool initialBusy)
  {
    return_false_if(!this->iocp || !connection || connection->Fd() == -1);

    auto message = std::make_unique<RegisterMessage>();
    message->type = MessageType::REG;
    message->connection = connection;
    message->onReceive = onReceive;
    message->initialBusy = initialBusy;
    message->completed = message->completeNotified = false;

    return_false_if(!this->messages.Produce(message.get()));
    auto msg = message.release();

    if (!this->NotifyMessage())
    {
      return false;
    }
    
    if (!onReceive)
    {
      // We are removing the callback. This may happen in a destructor. Wait until it is actually removed.
      if (!msg->completed)
      {
        std::unique_lock<std::mutex> lock(msg->mutex);
        if (!msg->completed)
        {
          msg->completeCond.wait(lock);
        }
      }

      {
        std::unique_lock<std::mutex> lock(msg->mutex);
        msg->completeNotified = true;
        msg->completeNotifiedCond.notify_all();
      }
    }

    return true;
  }


  void SocketDispatcherImpl::Delete(Socket fd)
  {
    return_if(!this->iocp || fd == -1);

    auto message = std::make_unique<DeleteMessage>();
    message->type = MessageType::DEL;
    message->fd = fd;

    return_if(!this->messages.Produce(message.get()));
    message.release();

    unused_result(this->NotifyMessage());
  }


  bool SocketDispatcherImpl::Send(SocketConnectionPtr connection, Buffer data)
  {
    return_false_if(!this->iocp || !connection || connection->Fd() == -1);

    auto message = std::make_unique<SendDataMessage>();
    message->type = MessageType::SEND;
    message->connection = connection;
    message->data = std::move(data);

    return_false_if(!this->messages.Produce(message.get()));
    message.release();

    return this->NotifyMessage();
  }


  void SocketDispatcherImpl::ThreadProc()
  {
    while (this->active)
    {
      DWORD nbytes = 0;
      ULONG_PTR ckey = 0;
      OVERLAPPED * overlapped = nullptr;
      if (!GetQueuedCompletionStatus(this->iocp, &nbytes, &ckey, &overlapped, INFINITE))
      {
        Log::Debug("SocketDispatcherImpl::ThreadProc: IOCP got event with winerr=%d", GetLastError());
        continue;
      }

      if (ckey == reinterpret_cast<ULONG_PTR>(this))
      {
        this->HandleMessages();
        continue;
      }

      Socket fd = static_cast<Socket>(ckey);
      auto itr = this->connections.find(fd);
      if (itr == this->connections.end())
      {
        Log::Debug("SocketDispatcherImpl: Got event from unknown fd %d.", fd);
        continue;
      }

      auto & entry = itr->second;
      size_t evt = 0;
      for (evt = 0; evt < sizeof(entry.contexts) / sizeof(CompletionContext); ++evt)
      {
        if (&entry.contexts[evt].overlapped == overlapped)
        {
          break;
        }
      }

      if (evt >= static_cast<size_t>(CompletionContextType::__MAX__))
      {
        Log::Debug("SocketDispatcherImpl: Got event from unknown type %u for fd $d.", evt, fd);
        continue;
      }

      switch (static_cast<CompletionContextType>(evt))
      {
        case CompletionContextType::CONNECT:
        {
          this->OnConnect(fd);
          break;
        }

        case CompletionContextType::SEND:
        {
          this->OnSend(fd, nbytes);
          break;
        }

        case CompletionContextType::RECV:
        {
          this->OnReceive(fd, nbytes);
          break;
        }

        default:
          break;
      }
    }

    for (const auto & connection : this->connections)
    {
      connection.second.connection->Close();
    }
  }
  

  bool SocketDispatcherImpl::ConnectSocket(Entry & entry)
  {
    static LPFN_CONNECTEX lpfnConnectEx = nullptr;
    if (!lpfnConnectEx)
    {
      DWORD nbytes = 0;
      GUID guidConnectEx = WSAID_CONNECTEX;
      if (SOCKET_ERROR == WSAIoctl(entry.connection->Fd(),
          SIO_GET_EXTENSION_FUNCTION_POINTER,
          &guidConnectEx, sizeof(guidConnectEx),
          &lpfnConnectEx, sizeof(lpfnConnectEx),
          &nbytes, nullptr, nullptr))
      {
        Log::Critical("SocketDispatcherImpl: Failed to load ConnectEx, error=%d.", WSAGetLastError());
        return false;
      }
    }

    if (!lpfnConnectEx(entry.connection->Fd(),
                       entry.connection->Addr(), entry.connection->AddrLen(),
                       nullptr, 0, nullptr,
                       &entry.contexts[static_cast<size_t>(CompletionContextType::CONNECT)].overlapped))
    {
      int errorCode = WSAGetLastError();
      if (errorCode != ERROR_IO_PENDING)
      {
        Log::Verbose("SocketDispatcherImpl::ConnectSocket: Failed to connect socket. this=%p, fd=%d, err=%d", this, entry.connection->Fd(), errorCode);
        return false;
      }
    }

    return true;
  }

  
  void SocketDispatcherImpl::HandleMessages()
  {
    Message * msg = nullptr;
    while (this->messages.Consume(msg))
    {
      switch (msg->type)
      {
        case MessageType::REG:
        {
          this->HandleMessage(static_cast<RegisterMessage *>(msg));
          break;
        }

        case MessageType::DEL:
        {
          this->HandleMessage(static_cast<DeleteMessage *>(msg));
          break;
        }

        case MessageType::SEND:
        {
          this->HandleMessage(static_cast<SendDataMessage *>(msg));
          break;
        }

        default:
          break;
      }

      delete msg;
    }
  }


  void SocketDispatcherImpl::HandleMessage(RegisterMessage * msg)
  {
    auto fd = msg->connection->Fd();

    auto itr = this->connections.find(fd);
    if (itr == this->connections.end())
    {
      sockaddr_in addr;
      ZeroMemory(&addr, sizeof(sockaddr_in));
      addr.sin_family = AF_INET;
      if (bind(fd, (sockaddr *)&addr, sizeof(sockaddr_in)) != 0)
      {
        Log::Verbose("SocketDispatcher: fd %d failed to bind to local address.", fd);
        return;
      }

      if (!CreateIoCompletionPort((HANDLE)fd, this->iocp, fd, 0))
      {
        Log::Verbose("SocketDispatcher: failed to register fd %d to completion port.", fd);
        return;
      }

      this->connections[fd] = Entry();
      itr = this->connections.find(fd);
      itr->second.connection = msg->connection;
      ZeroMemory(&itr->second.contexts, sizeof(itr->second.contexts));
      if (msg->initialBusy)
      {
        itr->second.busyOut = true;
      }
    }
    else if (itr->second.connection.get() != msg->connection.get())
    {
      Log::Warning("SocketDispatcher: fd %d has already been registered by another connection: target=%p registered=%p", fd, msg->connection.get(), itr->second.connection.get());
    }

    auto & entry = itr->second;
    if (entry.connection == msg->connection)
    {
      entry.onReceive = msg->onReceive;

      if (entry.connection->IsConnectNeeded())
      {
        entry.connection->SetConnecting(true);
        entry.connection->SetConnectNeeded(false);
        if (!this->ConnectSocket(entry))
        {
          Log::Verbose("SocketDispatcher: failed to connect to socket %d.", fd);
          this->HandleError(entry.connection);
        }
      }
      else if (!entry.connection->IsConnecting() && entry.onReceive)
      {
        // Kick off receive
        this->OnReceive(fd, 0);
      }
    }
    
    // Notify complete on removing callbacks
    if (!msg->onReceive)
    {
      {
        std::unique_lock<std::mutex> lock(msg->mutex);
        msg->completed = true;
        msg->completeCond.notify_all();
      }

      if (!msg->completeNotified)
      {
        std::unique_lock<std::mutex> lock(msg->mutex);
        if (!msg->completeNotified)
        {
          msg->completeNotifiedCond.wait(lock);
        }
      }
    }
  }


  void SocketDispatcherImpl::HandleMessage(DeleteMessage * msg)
  {
    auto fd = msg->fd;

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    if (itr->second.onReceive)
    {
      itr->second.onReceive(this, itr->second.connection.get(), nullptr, 0);
    }

    this->connections.erase(itr);
  }


  void SocketDispatcherImpl::HandleMessage(SendDataMessage * msg)
  {
    fail_if(!msg);

    auto fd = msg->connection->Fd();

    auto itr = this->connections.find(fd);
    if(itr == this->connections.end())
    {
      Log::Warning("SocketDispatcher: trying to send to unknown fd %d", fd);
      return;
    }

    auto & entry = itr->second;
    if(entry.connection != msg->connection)
    {
      Log::Warning("SocketDispatcher: trying to send to unknown connection for fd %d: target=%p registered=%p", fd, msg->connection.get(), entry.connection.get());
      return;
    }

    if (msg->data.Size() > 0)
    {
      entry.sendBuf.WriteBuffer(std::move(msg->data));
    }

    this->TrySend(entry);
  }


  void SocketDispatcherImpl::TrySend(Entry & entry)
  {
    auto fd = entry.connection->Fd();

    return_if(entry.busyOut || entry.connection->IsConnecting() || entry.connection->IsClosed());

    auto & context = entry.contexts[static_cast<size_t>(CompletionContextType::SEND)];

    int rtn = 0;
    while (rtn == 0 && entry.sendBuf.Size() > 0)
    {
      context.wsaSendBuf.len = entry.sendBuf.PeekBuffer(context.sendBuf, sizeof(context.sendBuf));
      context.wsaSendBuf.buf = context.sendBuf;
      DWORD offset = 0;

      rtn = WSASend(fd, &context.wsaSendBuf, 1, &offset, 0, &context.overlapped, nullptr);
      if (rtn == SOCKET_ERROR)
      {
        int errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING && errorCode != WSAEWOULDBLOCK)
        {
          Log::Verbose("SocketDispatcherImpl::TrySend: Failed to send data on fd %d. error=%d", fd, errorCode);
          this->HandleError(entry.connection);
          return;
        }
        else
        {
          entry.busyOut = true;
        }
      }
      else if (rtn == 0 && offset > 0)
      {
        entry.sendBuf.ReadBuffer(nullptr, offset);
      }
      else
      {
        break;
      }
    }

    if (entry.connection->IsShuttingDown() && entry.sendBuf.Size() == 0)
    {
      // We are ready to close this connection
      this->HandleError(entry.connection);
    }
  }


  void SocketDispatcherImpl::HandleError(SocketConnectionPtr connection)
  {
    fail_if(!connection);

    DeleteMessage args;
    args.type = MessageType::DEL;
    args.fd = connection->Fd();

    this->HandleMessage(&args);
  }


  void SocketDispatcherImpl::HandleError(Socket fd)
  {
    fail_if(fd == INVALID_SOCKET);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    this->HandleError(itr->second.connection);
  }


  void SocketDispatcherImpl::OnConnect(Socket fd)
  {
    fail_if(fd == INVALID_SOCKET);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    auto & entry = itr->second;
    return_if(!entry.connection->IsConnecting());

    entry.connection->SetConnecting(false);
    entry.busyOut = false;

    this->OnReceive(fd, 0);
    this->TrySend(entry);
  }


  void SocketDispatcherImpl::OnSend(Socket fd, size_t nbytes)
  {
    assert(fd != -1);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    auto & entry = itr->second;
    if (nbytes > 0)
    {
      entry.sendBuf.ReadBuffer(nullptr, nbytes);
    }

    entry.busyOut = false;

    this->TrySend(entry);
  }


  void SocketDispatcherImpl::OnReceive(Socket fd, size_t nbytes)
  {
    assert(fd != -1);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    auto & entry = itr->second;
    return_if(entry.connection->IsConnecting() || entry.connection->IsClosed());
    
    auto & context = entry.contexts[static_cast<size_t>(CompletionContextType::RECV)];

    if (nbytes > 0)
    {
      if (entry.onReceive)
      {
        entry.onReceive(this, entry.connection.get(), context.recvBuf, nbytes);
      }
    }

    int rtn = 0;
    while (rtn == 0)
    {
      context.wsaRecvBuf.buf = context.recvBuf;
      context.wsaRecvBuf.len = sizeof(context.recvBuf);

      DWORD nrecv = 0;
      DWORD flags = 0;
      rtn = WSARecv(fd, &context.wsaRecvBuf, 1, &nrecv, &flags, &context.overlapped, nullptr);
      if (rtn == SOCKET_ERROR)
      {
        int errorCode = WSAGetLastError();
        if (errorCode != WSA_IO_PENDING && errorCode != WSAEWOULDBLOCK)
        {
          Log::Verbose("SocketDispatcherImpl: Failed to recv on fd %d. error=%d", fd, errorCode);
          this->HandleError(entry.connection);
          return;
        }
      }
      else if (rtn == 0)
      {
        if (entry.onReceive)
        {
          entry.onReceive(this, entry.connection.get(), context.recvBuf, nrecv);
        }
      }
      else
      {
        break;
      }
    }

    if (entry.connection->IsShuttingDown() && entry.sendBuf.Size() == 0)
    {
      // We are ready to close this connection
      this->HandleError(entry.connection);
    }
  }


  bool SocketDispatcherImpl::NotifyMessage()
  {
    fail_false_if(!this->iocp);
    return PostQueuedCompletionStatus(this->iocp, 0, reinterpret_cast<ULONG_PTR>(this), &this->messageOverlapped) == TRUE;
  }

}
