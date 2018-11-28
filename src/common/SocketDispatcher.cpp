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

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <aeres/ScopeGuard.h>
#include <aeres/Log.h>
#include <aeres/Util.h>
#include <aeres/SocketDispatcher.h>

namespace aeres
{
  SocketDispatcher::~SocketDispatcher()
  {
    this->active = false;

    if (this->pipe[0] != -1)
    {
      uint8_t signal = 0;
      unused_result(write(this->pipe[0], &signal, sizeof(signal)) == sizeof(signal));

      close(this->pipe[0]);
      this->pipe[0] = -1;
    }

    if (this->thread.joinable())
    {
      this->thread.join();
    }

    this->eventQueue.reset();

    if (this->pipe[1] != -1)
    {
      close(this->pipe[1]);
      this->pipe[1] = -1;
    }

    Message * message = nullptr;
    while (this->messages.Consume(message))
    {
      if (message)
      {
        delete message;
      }
    }
  }


  bool SocketDispatcher::Initialize()
  {
    return_false_if(socketpair(AF_UNIX, SOCK_DGRAM, 0, this->pipe) == -1);

    this->thread = std::thread(std::bind(&SocketDispatcher::ThreadProc, this));

    return false;
  }


  bool SocketDispatcher::Register(SocketConnectionPtr connection, Listener onReceive, bool initialBusy)
  {
    return_false_if(this->pipe[0] == -1 || !connection || connection->Fd() == -1);

    auto message = std::make_unique<RegisterMessage>();
    message->type = MessageType::REGISTER;
    message->connection = connection;
    message->onReceive =onReceive;
    message->initialBusy = initialBusy;

    return_false_if(!this->messages.Produce(message.get()));
    message.release();

    uint8_t signal = 0;
    return write(this->pipe[0], &signal, sizeof(signal)) == sizeof(signal);
  }


  void SocketDispatcher::Delete(int fd)
  {
    return_if(this->pipe[0] == -1 || fd == -1);

    auto message = std::make_unique<DeleteMessage>();
    message->type = MessageType::DELETE;
    message->fd = fd;

    return_if(!this->messages.Produce(message.get()));
    message.release();

    uint8_t signal = 0;
    unused_result(write(this->pipe[0], &signal, sizeof(signal)));
  }


  bool SocketDispatcher::Send(SocketConnectionPtr connection, Buffer data)
  {
    return_false_if(this->pipe[0] == -1 || !connection || connection->Fd() == -1);

    auto message = std::make_unique<SendMessage>();
    message->type = MessageType::SEND;
    message->connection = connection;
    message->data = std::move(data);

    return_false_if(!this->messages.Produce(message.get()));
    message.release();

    uint8_t signal = 0;
    return write(this->pipe[0], &signal, sizeof(signal)) == sizeof(signal);
  }


  void SocketDispatcher::ThreadProc()
  {
    ScopeGuard guard([this]() {
      if (this->pipe[1] != -1)
      {
        close(this->pipe[1]);
        this->pipe[1] = -1;
      }
    });

    this->eventQueue = std::make_unique<IoEventQueue>();
    return_if(!this->eventQueue->Initialize());

    return_if(!this->eventQueue->AddSubscription(this->pipe[1], IoEventQueue::EventType::READ, false));

    const size_t MAXEVENTS = 10;
    IoEventQueue::Event events[MAXEVENTS];
    size_t nfd = MAXEVENTS;

    while(this->active && this->eventQueue->WaitForEvents(events, &nfd))
    {
      for (auto i = 0; i < nfd; ++i)
      {
        bool error = (events[i].type & IoEventQueue::EventType::ERROR);

        if (events[i].fd == this->pipe[1])
        {
          return_if(error || !(events[i].type & IoEventQueue::EventType::READ));

          uint8_t signal = 0;
          ssize_t rtn = read(this->pipe[1], &signal, 1);
          return_if(rtn == 0 || (rtn < 0 && errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR));

          this->HandleMessages();
        }
        else if (error)
        {
          this->HandleError(events[i].fd);
        }
        else
        {
          bool handled = false;

          if (events[i].type & IoEventQueue::EventType::WRITE)
          {
            this->OnSend(events[i].fd);
            handled = true;
          }

          if (events[i].type & IoEventQueue::EventType::READ)
          {
            this->OnReceive(events[i].fd);
            handled = true;
          }

          if (!handled)
          {
            this->HandleError(events[i].fd);
          }
        }
      }
    }
  }


  void SocketDispatcher::HandleMessages()
  {
    Message * msg = nullptr;
    while (this->messages.Consume(msg))
    {
      switch (msg->type)
      {
        case MessageType::REGISTER:
        {
          this->HandleMessage(static_cast<RegisterMessage *>(msg));
          break;
        }

        case MessageType::DELETE:
        {
          this->HandleMessage(static_cast<DeleteMessage *>(msg));
          break;
        }

        case MessageType::SEND:
        {
          this->HandleMessage(static_cast<SendMessage *>(msg));
          break;
        }

        default:
          break;
      }

      delete msg;
    }
  }


  void SocketDispatcher::HandleMessage(RegisterMessage * msg)
  {
    int fd = msg->connection->Fd();

    auto itr = this->connections.find(fd);
    if (itr == this->connections.end())
    {
      int eventType = IoEventQueue::EventType::READ;
      if (msg->initialBusy)
      {
        eventType |= IoEventQueue::EventType::WRITE;
      }

      if (!this->eventQueue->AddSubscription(fd, eventType, true))
      {
        Log::Warning("Failed to monitor fd %d. (err=%d)", fd, this->eventQueue->GetLastError());
        return;
      }

      this->connections[fd] = Entry();
      itr = this->connections.find(fd);
      itr->second.connection = msg->connection;
    }
    else if (itr->second.connection.get() != msg->connection.get())
    {
      Log::Warning("SocketDispatcher: fd %d has already been registered by another connection: target=%p registered=%p", fd, msg->connection.get(), itr->second.connection.get());
    }

    auto & entry = itr->second;
    return_if(entry.connection != msg->connection);

    entry.onReceive = msg->onReceive;

    // Immediately try receive once since we may have some missing messages before we set up the epoll
    this->OnReceive(fd);
  }


  void SocketDispatcher::HandleMessage(DeleteMessage * msg)
  {
    int fd = msg->fd;

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    if (!this->eventQueue->RemoveSubscription(fd))
    {
      Log::Warning("SocketDispatcher: Failed to remove monitor for fd: fd=%d", fd);
    }

    if (itr->second.onReceive)
    {
      itr->second.onReceive(this, itr->second.connection.get(), nullptr, 0);
    }

    this->connections.erase(itr);
  }


  void SocketDispatcher::HandleMessage(SendMessage * msg)
  {
    assert(msg);

    int fd = msg->connection->Fd();

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


  void SocketDispatcher::TrySend(Entry & entry)
  {
    int fd = entry.connection->Fd();

    return_if(entry.busyOut);

    while (entry.sendBuf.Size() > 0)
    {
      uint8_t buf[8192];
      size_t size = entry.sendBuf.PeekBuffer(buf, sizeof(buf));
      size_t offset = 0;
      ssize_t len = 0;

      while (offset < size)
      {
        if (((len = write(fd, buf + offset, size - offset)) < 0 && errno != EINTR) || len == 0)
        {
          break;
        }

        if (len > 0)
        {
          offset += len;
        }
      }

      int error = errno;

      if (offset > 0)
      {
        entry.sendBuf.ReadBuffer(nullptr, offset);
      }

      if (len < 0 && (error == EAGAIN || error == EWOULDBLOCK))
      {
        this->SetSendBusy(entry, true);
      }
      else if (len <= 0)
      {
        this->HandleError(entry.connection);
        break;
      }
    }

    if (entry.connection->IsShuttingDown() && entry.sendBuf.Size() == 0)
    {
      // We are ready to close this connection
      this->HandleError(entry.connection);
    }
  }


  void SocketDispatcher::SetSendBusy(Entry & entry, bool busy)
  {
    return_if(entry.busyOut == busy);

    int fd = entry.connection->Fd();

    entry.busyOut = busy;

    int eventType = IoEventQueue::EventType::READ;
    if (busy)
    {
      eventType |= IoEventQueue::EventType::WRITE;
    }

    this->eventQueue->UpdateSubscription(fd, eventType, true);
  }


  void SocketDispatcher::HandleError(SocketConnectionPtr connection)
  {
    assert(connection);

    DeleteMessage args;
    args.type = MessageType::DELETE;
    args.fd = connection->Fd();

    this->HandleMessage(&args);
  }


  void SocketDispatcher::HandleError(int fd)
  {
    assert(fd != -1);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    this->HandleError(itr->second.connection);
  }


  void SocketDispatcher::OnSend(int fd)
  {
    assert(fd != -1);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    auto & entry = itr->second;
    this->SetSendBusy(entry, false);

    this->TrySend(entry);
  }


  void SocketDispatcher::OnReceive(int fd)
  {
    assert(fd != -1);

    auto itr = this->connections.find(fd);
    return_if(itr == this->connections.end());

    auto & entry = itr->second;

    uint8_t buffer[BUFSIZ];
    ssize_t len = 0;

    while ((len = read(fd, buffer, sizeof(buffer))) > 0 || (len < 0 && errno == EINTR))
    {
      if (len > 0 && entry.onReceive)
      {
        entry.onReceive(this, entry.connection.get(), buffer, len);
      }
    }

    if (len == 0 || (len < 0 && errno != EAGAIN && errno != EWOULDBLOCK))
    {
      this->HandleError(entry.connection);
    }
  }
}
