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

#include <functional>
#include <net/quic/quartc/quartc_session.h>

namespace aeres
{
  namespace quic
  {
    class QuartcSessionDelegate : public net::QuartcSessionInterface::Delegate
    {
    public:

      using OnCryptoHandshakeCompleteCallback = std::function<void()>;
      using OnIncomingStreamCallback = std::function<void(net::QuartcStreamInterface *)>;
      using OnConnectionClosedCallback = std::function<void(int, bool)>;

      void SetCryptoHandshakeCompleteCallback(OnCryptoHandshakeCompleteCallback callback)
      {
        this->onCryptoHandshakeComplete = callback;
      }

      void SetIncomingStreamCallback(OnIncomingStreamCallback callback)
      {
        this->onIncomingStream = callback;
      }

      void SetConnectionClosedCallback(OnConnectionClosedCallback callback)
      {
        this->onConnectionClosed = callback;
      }

    public:

      void OnCryptoHandshakeComplete() override
      {
        if (this->onCryptoHandshakeComplete)
        {
          this->onCryptoHandshakeComplete();
        }
      }

      void OnIncomingStream(net::QuartcStreamInterface * stream) override
      {
        if (this->onIncomingStream)
        {
          this->onIncomingStream(stream);
        }
      }

      void OnConnectionClosed(int errorCode, bool fromRemote) override
      {
        if (this->onConnectionClosed)
        {
          this->onConnectionClosed(errorCode, fromRemote);
        }
      }

    private:

      OnCryptoHandshakeCompleteCallback onCryptoHandshakeComplete = nullptr;

      OnIncomingStreamCallback onIncomingStream = nullptr;

      OnConnectionClosedCallback onConnectionClosed = nullptr;
    };
  }
}
