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

#include <map>
#include <set>
#include <thread>
#include <memory>
#include <atomic>
#include <aeres/Buffer.h>
#include <aeres/BufferStream.h>
#include <aeres/LockFreeQueue.h>
#include <aeres/IoEventQueue.h>
#include <aeres/SocketConnection.h>

namespace aeres
{
  class SocketDispatcher
  {
  public:

    using Listener = std::function<void(SocketDispatcher *, SocketConnection *, const void *, size_t)>;

  private:

    enum class MessageType
    {
      REGISTER,
      DELETE,
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
    };

    struct DeleteMessage : public Message
    {
      int fd;
    };

    struct SendMessage : public Message
    {
      Buffer data;
      SocketConnectionPtr connection;
    };

    struct Entry
    {
      SocketConnectionPtr connection;
      Listener onReceive;
      BufferStream sendBuf;
      bool busyOut = false;
    };

  public:

    ~SocketDispatcher();

    bool Initialize();

    bool Register(SocketConnectionPtr connection, Listener onReceive, bool initialBusy = false);

    void Delete(int fd);

    bool Send(SocketConnectionPtr connection, Buffer data);

  private:

    void ThreadProc();

    void HandleMessages();
    void HandleMessage(RegisterMessage * msg);
    void HandleMessage(DeleteMessage * msg);
    void HandleMessage(SendMessage * msg);

    void TrySend(Entry & entry);
    void SetSendBusy(Entry & entry, bool busy);

    void HandleError(int fd);
    void HandleError(SocketConnectionPtr connection);

    void OnSend(int fd);
    void OnReceive(int fd);

  private:

    std::unique_ptr<IoEventQueue> eventQueue;

    std::map<int, Entry> connections;

    LockFreeQueue<Message *> messages;

    std::thread thread;

    int pipe[2] = { -1, -1 };

    std::atomic<bool> active{true};
  };
}
