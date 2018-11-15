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
#include <functional>
#include <atomic>
#include <mutex>
#include <base/memory/weak_ptr.h>

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
  class ConnectionFactory;


  namespace client
  {
    class QuicClientSession
    {
    public:

      using OnCloseCallback = std::function<void(QuicClientSession *)>;

    public:

      explicit QuicClientSession(ConnectionFactory * factory);

      ~QuicClientSession();

      bool Initialize(net::QuicRawSession * session, net::QuartcStream * stream);

      void SetCloseCallback(OnCloseCallback callback)   { this->onClose = callback; }

      void Close();

    private:

      void OnQuicReceived(const void * data, size_t len);

      void OnHandshake(uint16_t port);

      void OnLocalData(std::shared_ptr<Connection> connectionPtr,
                       std::shared_ptr<Connection> localPtr,
                       const void * data,
                       size_t len);

      void CloseRemote(std::shared_ptr<Connection> connectionPtr, std::shared_ptr<Connection> localPtr);

      void CloseLocal(std::shared_ptr<Connection> localPtr, std::shared_ptr<Connection> connectionPtr);

    private:

      std::shared_ptr<Connection> connection;

      OnCloseCallback onClose = nullptr;

      ConnectionFactory * factory = nullptr;

      std::shared_ptr<Connection> local;

      std::atomic<bool> closeRemote{false};

      std::atomic<bool> closeLocal{false};

      base::TaskRunner * taskRunner = nullptr;

      base::WeakPtrFactory<QuicClientSession> weakFactory;

    };
  }
}
