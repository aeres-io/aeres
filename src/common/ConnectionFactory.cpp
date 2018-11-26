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

#ifndef SOCK_NONBLOCK
#include <fcntl.h>
#define SOCK_NONBLOCK O_NONBLOCK
#define SOCK_CLOEXEC O_CLOEXEC
#endif

#include <assert.h>
#include <atomic>
#include <unistd.h>
#include <netinet/in.h>
#include <aeres/ScopeGuard.h>
#include <aeres/Util.h>
#include <aeres/SocketConnection.h>
#include <aeres/SocketDispatcher.h>
#include <aeres/Config.h>
#include <aeres/Log.h>
#include <aeres/ConnectionFactory.h>

namespace aeres
{
  struct SocketDispatcherWrapper
  {
    SocketDispatcher dispatcher;
    std::atomic<int> nrefs{1};
  };

  static SocketDispatcherWrapper * g_socketDispatcher = nullptr;


  ConnectionFactory::ConnectionFactory()
  {
    if (!g_socketDispatcher)
    {
      g_socketDispatcher = new SocketDispatcherWrapper();
      g_socketDispatcher->dispatcher.Initialize();
    }
  }


  ConnectionFactory::~ConnectionFactory()
  {
    return_if(!g_socketDispatcher);

    if (--g_socketDispatcher->nrefs <= 0)
    {
      delete g_socketDispatcher;
      g_socketDispatcher = nullptr;
    }
  }


  std::shared_ptr<Connection> ConnectionFactory::CreateSocketConnection(int fd, bool autoStart) const
  {
    return_val_if(nullptr, !g_socketDispatcher);

    SetSocketOptions(fd);

    auto conn = std::make_shared<SocketConnection>(&g_socketDispatcher->dispatcher, fd);

    Log::Debug("ConnectionFactory::CreateSocketConnection: fd=%d autoStart=%d", fd, autoStart ? 1 : 0);

    if (autoStart)
    {
      return_val_if(nullptr, !g_socketDispatcher->dispatcher.Register(conn, nullptr));
    }

    return std::static_pointer_cast<Connection>(conn);
  }


  std::shared_ptr<Connection> ConnectionFactory::CreateSocketConnection(struct sockaddr * addr, socklen_t len, bool autoStart) const
  {
    return_val_if(nullptr, !g_socketDispatcher || !addr );

    int fd = -1;

    ScopeGuard guard([fd]() {
      if (fd != -1)
      {
        close(fd);
      }
    });

    fd = socket(addr->sa_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    return_val_if(nullptr, fd == -1);

    SetSocketOptions(fd);

    return_val_if(nullptr, connect(fd, addr, len) == -1 && errno != EINPROGRESS);

    guard.Reset();

    auto conn = std::make_shared<SocketConnection>(&g_socketDispatcher->dispatcher, fd);

    if (autoStart)
    {
      return_val_if(nullptr, !g_socketDispatcher->dispatcher.Register(conn, nullptr, true));
    }

    return std::static_pointer_cast<Connection>(conn);
  }


  void ConnectionFactory::SetSocketOptions(int fd)
  {
    int flags = 1;
    setsockopt(fd, IPPROTO_TCP, 1 /* TCP_NODELAY */, &flags, sizeof(flags));
    setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));

    struct timeval tv = {0};
    tv.tv_sec = Config::SendTimeout();
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

    tv.tv_sec = Config::RecvTimeout();
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  }
}
