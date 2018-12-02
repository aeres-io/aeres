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


#include <assert.h>
#include <aeres/Util.h>
#include <aeres/Log.h>
#include <aeres/SocketDispatcher.h>
#include <aeres/SocketConnection.h>


namespace aeres
{
  SocketConnection::SocketConnection(SocketDispatcher * dispatcher, Socket fd)
    : dispatcher(dispatcher)
    , fd(fd)
    , closed(false)
    , closing(false)
#ifdef WIN32
    , connectNeeded(false)
    , connecting(false)
#endif
#ifdef DEBUG
    , totalBytesSent(0)
    , totalBytesReceived(0)
#endif
  {
    assert(dispatcher && IsSocketValid(fd));

    Log::Debug("SocketConnection::SocketConnection: this=%p", this);
  }


  SocketConnection::~SocketConnection()
  {
    Log::Debug("SocketConnection:~SocketConnection: this=%p", this);
    this->Close();

#ifdef WIN32
    this->SetAddr(nullptr, 0);
#endif
  }


  bool SocketConnection::Send(Buffer buffer)
  {
    return_false_if(!IsSocketValid(this->fd));

    Log::Debug("SocketConnection::Send: fd=%d buffer.size=%llu", this->fd, (long long unsigned)buffer.Size());

    auto ptr = std::static_pointer_cast<SocketConnection>(shared_from_this());

#ifdef DEBUG
    this->totalBytesSent += buffer.Size();
    Log::Debug("SocketConnction: totalBytesSent=%llu totalBytesReceived=%llu",
      (long long unsigned)this->totalBytesSent,
      (long long unsigned)this->totalBytesReceived);
#endif

    return this->dispatcher->Send(ptr, std::move(buffer));
  }


  bool SocketConnection::SetRecvCallback(RecvCallback callback)
  {
    return_false_if(!IsSocketValid(this->fd));

    SocketDispatcher::Listener listener = nullptr;

    if (callback)
    {
      listener = [this, callback](SocketDispatcher * sender,
                                  SocketConnection * conn,
                                  const void * data,
                                  size_t len)
      {
        Log::Debug("SocketConnection::onReceived: fd=%d len=%llu", this->fd, (long long unsigned)len);
#ifdef DEBUG
        this->totalBytesReceived += len;
        Log::Debug("SocketConnction: totalBytesSent=%llu totalBytesReceived=%llu",
          (long long unsigned)this->totalBytesSent,
          (long long unsigned)this->totalBytesReceived);
#endif
        callback(this, data, len);
      };
    }

    auto ptr = std::static_pointer_cast<SocketConnection>(shared_from_this());

    return this->dispatcher->Register(ptr, listener);
  }


  void SocketConnection::Close()
  {
    this->closed = true;

    return_if(!IsSocketValid(this->fd));

    ShutdownSocket(this->fd);
    CloseSocket(this->fd);

    Log::Verbose("Socket connection closed: fd=%d", this->fd);

    this->dispatcher->Delete(this->fd);

    this->fd = INVALID_SOCKET;
  }

  void SocketConnection::Shutdown()
  {
    this->closing = true;

    // Send a 0-sized buffer to notify the dispatcher of shutdown event
    Buffer buffer;
    auto ptr = std::static_pointer_cast<SocketConnection>(shared_from_this());
    this->dispatcher->Send(ptr, std::move(buffer));
  }
}
