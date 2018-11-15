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

namespace aeres
{
  class SocketDispatcher;

  class SocketConnection : public Connection
  {
  public:

    SocketConnection(SocketDispatcher * dispatcher, int fd);

    ~SocketConnection() override;

    bool Send(Buffer buffer) override;

    bool SetRecvCallback(RecvCallback callback) override;

    void Close() override;

    void Shutdown() override;

    int Fd() const                  { return this->fd; }

    bool IsClosed() const override  { return this->closed; }

    bool IsShuttingDown() const     { return this->closing; }

  private:

    SocketDispatcher * dispatcher;

    int fd;

    std::atomic<bool> closed;

    std::atomic<bool> closing;

#ifdef DEBUG

    std::atomic<size_t> totalBytesSent;

    std::atomic<size_t> totalBytesReceived;

#endif
  };

  using SocketConnectionPtr = std::shared_ptr<SocketConnection>;
}
