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

#include <atomic>
#include <base/memory/weak_ptr.h>
#include <aeres/quic/QuartcStreamDelegate.h>
#include <aeres/Connection.h>

namespace base
{
  class TaskRunner;
}

namespace net
{
  class QuicRawSession;
  class QuartcStream;
}

namespace aeres
{
  namespace quic
  {
    class QuicConnection : public Connection
    {
    public:

      explicit QuicConnection(net::QuicRawSession * session, net::QuartcStream * stream);

      ~QuicConnection() override;

      bool Send(Buffer buffer) override;

      bool SetRecvCallback(RecvCallback callback) override;

      void Close() override;

      void Shutdown() override;

      net::QuicRawSession * Session() const   { return this->session; }

      net::QuartcStream * Stream() const      { return this->stream; }

      bool IsClosed() const override          { return this->closed; }

      bool IsShuttingDown() const             { return this->closing; }

    private:

      void OnSend(Buffer buffer);

      static void OnClose(net::QuartcStream * s, bool closeStream);

    private:

      net::QuicRawSession * session = nullptr;

      std::atomic<net::QuartcStream *> stream{nullptr};

      RecvCallback onReceived = nullptr;

      quic::QuartcStreamDelegate streamDelegate;

      base::TaskRunner * taskRunner = nullptr;

      base::WeakPtrFactory<QuicConnection> weakFactory;

      std::atomic<bool> closed{false};

      std::atomic<bool> closing{false};

      static quic::QuartcStreamDelegate * defaultStreamDelegate;

#ifdef DEBUG

      std::atomic<size_t> totalBytesSent{0};

      std::atomic<size_t> totalBytesReceived{0};

      std::atomic<size_t> writeCount{0};

      std::atomic<size_t> readCount{0};
#endif
    };
  }
}
