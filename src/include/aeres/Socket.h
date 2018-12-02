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

#pragma once

#ifndef WIN32
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#else
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <Windows.h>
#include <aeres/PlatformUtil.h>
#endif


namespace aeres
{
#ifndef WIN32
  using Socket = int;
  enum { INVALID_SOCKET = -1 };
#else
  using Socket = SOCKET;
  using socklen_t = int;
#endif

  inline bool IsSocketValid(Socket handle)
  {
    return handle != INVALID_SOCKET;
  }


  inline int CloseSocket(Socket handle)
  {
#ifndef WIN32
    return close(handle);
#else
    return closesocket(handle);
#endif
  }


  inline int ShutdownSocket(Socket handle)
  {
#ifndef WIN32
    return shutdown(handle, SHUT_RDWR);
#else
    return shutdown(handle, SD_BOTH);
#endif
  }


  inline int SetSockOpt(Socket handle, int level, int name, const void * value, socklen_t len)
  {
#ifndef WIN32
    return setsockopt(handle, level, name, value, len);
#else
    return setsockopt(handle, level, name, static_cast<const char *>(value), len);
#endif
  }


  inline int GetLastSocketError()
  {
#ifndef WIN32
    return errno;
#else
    return WSAGetLastError();
#endif
  }
}