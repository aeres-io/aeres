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
#include <errno.h>
#include <sys/event.h>
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
    this->handle = kqueue();
    return this->handle != -1;
  }


  static bool updateEvents(int handle, int fd, int eventType, bool oneShot, bool modify)
  {
    struct kevent evts[2];
    int nfd = 0;
    if (eventType & IoEventQueue::EventType::READ)
    {
      EV_SET(&evts[nfd++], fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, (void *)(intptr_t)fd);
    }
    else if (modify)
    {
      EV_SET(&evts[nfd++], fd, EVFILT_READ, EV_DELETE, 0, 0, (void *)(intptr_t)fd);
    }

    int oneShotFlag = oneShot ? EV_ONESHOT : 0;
    if (eventType & IoEventQueue::EventType::WRITE)
    {
      EV_SET(&evts[nfd++], fd, EVFILT_WRITE, EV_ADD | EV_ENABLE | oneShotFlag, 0, 0, (void *)(intptr_t)fd);
    }
    else if (modify)
    {
      EV_SET(&evts[nfd++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *)(intptr_t)fd);
    }

    return kevent(handle, evts, nfd, nullptr, 0, nullptr) != -1;
  }


  bool IoEventQueue::AddSubscription(int fd, int eventType, bool oneShot)
  {
    return_false_if(this->handle == -1 || eventType == 0);
    return updateEvents(this->handle, fd, eventType, oneShot, false);
  }


  bool IoEventQueue::UpdateSubscription(int fd, int eventType, bool oneShot)
  {
    return_false_if(this->handle == -1 || eventType == 0);
    return updateEvents(this->handle, fd, eventType, oneShot, true);
  }


  bool IoEventQueue::RemoveSubscription(int fd)
  {
    return_false_if(this->handle == -1);

    struct kevent evts[2];
    EV_SET(&evts[0], fd, EVFILT_READ, EV_DELETE, 0, 0, (void *)(intptr_t)fd);
    EV_SET(&evts[1], fd, EVFILT_WRITE, EV_DELETE, 0, 0, (void *)(intptr_t)fd);

    return kevent(this->handle, evts, 2, nullptr, 0, nullptr) != -1;
  }


  bool IoEventQueue::WaitForEvents(IoEventQueue::Event * events, size_t * eventsLen)
  {
    return_false_if(this->handle == -1 || !events || !eventsLen);

    if (*eventsLen > this->eventBufSize)
    {
      void * buf = realloc(this->eventBuf, (*eventsLen) * sizeof(struct kevent));
      if (buf)
      {
        this->eventBuf = buf;
        this->eventBufSize = *eventsLen;
      }
      
      return_false_if(!this->eventBuf);
    }

    struct kevent * evts = static_cast<struct kevent *>(this->eventBuf);
    int nfd = kevent(this->handle, nullptr, 0, evts, static_cast<int>(this->eventBufSize), nullptr);

    return_false_if(nfd <= 0 && errno != EINTR);

    if (nfd <= 0)
    {
      *eventsLen = 0;
      return true;
    }

    *eventsLen = static_cast<size_t>(nfd);
    for (int i = 0; i < nfd; ++i)
    {
      events[i].fd = (int)(intptr_t)evts[i].udata;
      events[i].type = 0;
      if (evts[i].flags & EV_ERROR)
      {
        events[i].type |= EventType::ERROR;
      }

      if (evts[i].filter == EVFILT_WRITE)
      {
        events[i].type |= EventType::WRITE;
      }

      if (evts[i].filter == EVFILT_READ)
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
