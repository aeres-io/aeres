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

#include <memory>
#include <atomic>
#include <aeres/Connection.h>
#include <aeres/Socket.h>

namespace aeres
{
  class SocketDispatcher;

  class SocketConnection : public Connection
  {
  public:

    SocketConnection(SocketDispatcher * dispatcher, Socket fd);

    ~SocketConnection() override;

    bool Send(Buffer buffer) override;

    bool SetRecvCallback(RecvCallback callback) override;

    void Close() override;

    void Shutdown() override;

    Socket Fd() const               { return this->fd; }

    bool IsClosed() const override  { return this->closed; }

    bool IsShuttingDown() const     { return this->closing; }

    size_t PendingSend() const      { return this->pendingSend; }

    void IncreasePendingSend(size_t val);

    void DecreasePendingSend(size_t val);

#ifdef WIN32
    bool IsConnectNeeded() const    { return this->connectNeeded; }

    bool IsConnecting() const       { return this->connecting; }

    void SetConnectNeeded(bool val) { this->connectNeeded = val; }

    void SetConnecting(bool val)    { this->connecting = val; }

    struct sockaddr * Addr() const  { return this->addr; }

    socklen_t AddrLen() const       { return this->addrLen; }

    void SetAddr(const struct sockaddr * val, socklen_t len)
    {
      if (this->addr)
      {
        delete reinterpret_cast<uint8_t *>(this->addr);
        this->addr = nullptr;
        this->addrLen = 0;
      }

      if (val && len > 0)
      {
        this->addr = reinterpret_cast<struct sockaddr *>(new uint8_t[len]);
        this->addrLen = len;
        memcpy(this->addr, val, len);
      }
    }
#endif

  private:

    SocketDispatcher * dispatcher;

    Socket fd;

    std::atomic<bool> closed;

    std::atomic<bool> closing;

    std::atomic<size_t> pendingSend;

#ifdef WIN32
    std::atomic<bool> connectNeeded;
    std::atomic<bool> connecting;
    struct sockaddr * addr = nullptr;
    socklen_t addrLen = 0;
#endif

#ifdef DEBUG

    std::atomic<size_t> totalBytesSent;

    std::atomic<size_t> totalBytesReceived;

#endif
  };

  using SocketConnectionPtr = std::shared_ptr<SocketConnection>;
}
