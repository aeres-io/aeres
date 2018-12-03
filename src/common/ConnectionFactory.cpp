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

#ifdef OS_MACOSX
#include <fcntl.h>
#define SOCK_NONBLOCK O_NONBLOCK
#define SOCK_CLOEXEC O_CLOEXEC
#endif // OS_MACOSX

#include <assert.h>
#include <atomic>
#include <aeres/ScopeGuard.h>
#include <aeres/Util.h>
#include <aeres/Socket.h>
#include <aeres/SocketConnection.h>
#include <aeres/Config.h>
#include <aeres/Log.h>
#include <aeres/ConnectionFactory.h>

#ifndef WIN32
#include <netinet/in.h>
#include "SocketDispatcherPosix.h"
#else
#include "SocketDispatcherWin.h"
#endif

namespace aeres
{
  struct SocketDispatcherWrapper
  {
    SocketDispatcherImpl dispatcher;
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
//      delete g_socketDispatcher;
//      g_socketDispatcher = nullptr;
    }
  }


  std::shared_ptr<Connection> ConnectionFactory::CreateSocketConnection(Socket fd, bool autoStart) const
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

    Socket fd = INVALID_SOCKET;

    ScopeGuard guard([fd]() {
      if (IsSocketValid(fd))
      {
        CloseSocket(fd);
      }
    });

#ifdef __linux__
    fd = socket(addr->sa_family, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0);
    return_val_if(nullptr, !IsSocketValid(fd));
#elif defined(WIN32)
    fd = socket(addr->sa_family, SOCK_STREAM, IPPROTO_TCP);
    return_val_if(nullptr, !IsSocketValid(fd));
#else
    fd = socket(addr->sa_family, SOCK_STREAM, 0);
    return_val_if(nullptr, !IsSocketValid(fd));

    int flags = fcntl(fd, F_GETFL, 0);
    return_val_if(nullptr, flags < 0);
    return_val_if(nullptr, fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0);
#endif

    SetSocketOptions(fd);

#ifndef WIN32
    return_val_if(nullptr, connect(fd, addr, len) == -1 && errno != EINPROGRESS);
#endif

    guard.Reset();

    auto conn = std::make_shared<SocketConnection>(&g_socketDispatcher->dispatcher, fd);

#ifdef WIN32
    conn->SetConnectNeeded(true);
    conn->SetAddr(addr, len);
#endif

    if (autoStart)
    {
      return_val_if(nullptr, !g_socketDispatcher->dispatcher.Register(conn, nullptr, true));
    }

    return std::static_pointer_cast<Connection>(conn);
  }


  void ConnectionFactory::SetSocketOptions(Socket fd)
  {
#ifndef WIN32
    int flags = 1;
#else
    BOOL flags = 1;
#endif
    SetSockOpt(fd, IPPROTO_TCP, 1 /* TCP_NODELAY */, &flags, sizeof(flags));
    SetSockOpt(fd, SOL_SOCKET, SO_KEEPALIVE, &flags, sizeof(flags));
    
#ifndef WIN32
    struct timeval tv = {0};
    tv.tv_sec = Config::SendTimeout();
#else
    DWORD tv = Config::SendTimeout() * 1000;
#endif
    SetSockOpt(fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));

#ifndef WIN32
    tv.tv_sec = Config::RecvTimeout();
#else
    tv = Config::RecvTimeout() * 1000;
#endif
    SetSockOpt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
  }
}
