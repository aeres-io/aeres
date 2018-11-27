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

#include <stdlib.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <aeres/Util.h>
#include <aeres/IoEventQueue.h>


namespace aeres
{
  IoEventQueue::~IoEventQueue()
  {
    if (this->handle != -1)
    {
      close(this->handle);
      this->handle = -1;
    }

    if (this->eventBuf)
    {
      free(this->eventBuf);
      this->eventBuf = nullptr;
    }
  }


  bool IoEventQueue::Initialize()
  {
    this->handle = epoll_create1(EPOLL_CLOEXEC);
    return this->handle != -1;
  }


  static void createEpollEvent(int fd, int eventType, bool oneShot, struct epoll_event & evt)
  {
    evt.data.fd = fd;
    evt.events = 0;
    if (eventType & IoEventQueue::EventType::READ)
    {
      evt.events |= EPOLLIN;
    }

    if (eventType & IoEventQueue::EventType::WRITE)
    {
      evt.events |= EPOLLOUT;
    }

    if (oneShot)
    {
      evt.events |= EPOLLET;
    }
  }


  bool IoEventQueue::AddSubscription(int fd, int eventType, bool oneShot)
  {
    return_false_if(this->handle == -1 || eventType == 0);

    struct epoll_event evt = {0};
    createEpollEvent(fd, eventType, oneShot, evt);

    return epoll_ctl(this->handle, EPOLL_CTL_ADD, fd, &evt) != -1;
  }


  bool IoEventQueue::UpdateSubscription(int fd, int eventType, bool oneShot)
  {
    return_false_if(this->handle == -1 || eventType == 0);

    struct epoll_event evt = {0};
    createEpollEvent(fd, eventType, oneShot, evt);

    return epoll_ctl(this->handle, EPOLL_CTL_MOD, fd, &evt) != -1;
  }


  bool IoEventQueue::RemoveSubscription(int fd)
  {
    return_false_if(this->handle == -1);

    struct epoll_event evt = {0};
    return epoll_ctl(this->handle, EPOLL_CTL_DEL, fd, &evt) != -1;
  }


  bool IoEventQueue::WaitForEvents(IoEventQueue::Event * events, size_t * eventsLen)
  {
    return_false_if(this->handle == -1 || !events || !eventsLen);

    if (*eventsLen > this->eventBufSize)
    {
      void * buf = realloc(this->eventBuf, (*eventsLen) * sizeof(struct epoll_event));
      if (buf)
      {
        this->eventBuf = buf;
        this->eventBufSize = *eventsLen;
      }
      
      return_false_if(!this->eventBuf);
    }

    struct epoll_event * evts = static_cast<struct epoll_event *>(this->eventBuf);
    int nfd = epoll_wait(this->handle, evts, this->eventBufSize, -1);
    return_false_if(nfd <= 0 && errno != EINTR);

    if (nfd <= 0)
    {
      *eventsLen = 0;
      return true;
    }

    *eventsLen = static_cast<size_t>(nfd);
    for (int i = 0; i < nfd; ++i)
    {
      events[i].fd = evts[i].data.fd;
      events[i].type = 0;
      if ((evts[i].events & EPOLLERR) || (evts[i].events & EPOLLHUP))
      {
        events[i].type |= EventType::ERROR;
      }

      if (evts[i].events & EPOLLOUT)
      {
        events[i].type |= EventType::WRITE;
      }

      if (evts[i].events & EPOLLIN)
      {
        events[i].type |= EventType::READ;
      }
    }

    return true;
  }


  int IoEventQueue::GetLastError()
  {
    return errno;
  }
}