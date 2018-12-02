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
#error("SocketDispatcherWin.h should only be included in WINDOWS build.")
#endif

#include <map>
#include <thread>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <aeres/LockFreeQueue.h>
#include <aeres/BufferStream.h>
#include <aeres/SocketDispatcher.h>

namespace aeres
{
  class SocketDispatcherImpl : public SocketDispatcher
  {
  private:

    enum class MessageType
    {
      REG,
      DEL,
      SEND
    };

    struct Message
    {
      MessageType type;

      virtual ~Message() = default;
    };

    struct RegisterMessage : public Message
    {
      Listener onReceive;
      SocketConnectionPtr connection;
      bool initialBusy;
      std::mutex mutex;
      std::condition_variable completeCond;
      std::condition_variable completeNotifiedCond;
      std::atomic<bool> completed;
      std::atomic<bool> completeNotified;
    };

    struct DeleteMessage : public Message
    {
      Socket fd;
    };

    struct SendDataMessage : public Message
    {
      Buffer data;
      SocketConnectionPtr connection;
    };


    enum class CompletionContextType
    {
      CONNECT = 0,
      RECV = 1,
      SEND = 2,
      __MAX__ = 3
    };

    struct CompletionContext
    {
      WSABUF wsaSendBuf;
      WSABUF wsaRecvBuf;
      CHAR sendBuf[8192];
      CHAR recvBuf[8192];
      WSAOVERLAPPED overlapped;
    };

    struct Entry
    {
      SocketConnectionPtr connection;
      Listener onReceive;
      BufferStream sendBuf;
      bool busyOut = false;
      CompletionContext contexts[static_cast<const size_t>(CompletionContextType::__MAX__)];
    };

  public:

    ~SocketDispatcherImpl() override;

    bool Initialize() override;

    bool Register(SocketConnectionPtr connection, Listener onReceive, bool initialBusy = false) override;

    void Delete(Socket fd) override;

    bool Send(SocketConnectionPtr connection, Buffer data) override;

  private:

    void ThreadProc();

    bool NotifyMessage();

    void HandleMessages();
    void HandleMessage(RegisterMessage * msg);
    void HandleMessage(DeleteMessage * msg);
    void HandleMessage(SendDataMessage * msg);

    void TrySend(Entry & entry);

    void HandleError(Socket fd);
    void HandleError(SocketConnectionPtr connection);

    bool ConnectSocket(Entry & entry);

    void OnConnect(Socket fd);
    void OnSend(Socket fd, size_t nbytes);
    void OnReceive(Socket fd, size_t nbytes);

  private:

    HANDLE iocp = NULL;

    std::map<Socket, Entry> connections;

    LockFreeQueue<Message *> messages;

    std::thread thread;
    
    std::atomic<bool> active{ true };

    OVERLAPPED messageOverlapped;
  };
}