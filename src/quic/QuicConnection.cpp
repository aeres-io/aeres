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
#include <base/task_runner.h>
#include <base/bind_helpers.h>
#include <base/threading/thread_task_runner_handle.h>
#include <net/tools/quic/relay/quic_raw_session.h>
#include <net/quic/quartc/quartc_stream.h>

#include <aeres/Util.h>
#include <aeres/Log.h>
#include <aeres/quic/QuicConnection.h>

namespace aeres
{
  namespace quic
  {
    quic::QuartcStreamDelegate * QuicConnection::defaultStreamDelegate = nullptr;


    QuicConnection::QuicConnection(net::QuicRawSession * session, net::QuartcStream * stream)
      : session(session)
      , stream(stream)
      , weakFactory(this)
    {
      assert(session);
      assert(stream);

      if (!defaultStreamDelegate)
      {
        defaultStreamDelegate = new quic::QuartcStreamDelegate();
      }

      Log::Debug("QuicConnection::QuicConnection: this=%p", this);

      this->taskRunner = base::ThreadTaskRunnerHandle::Get().get();

      this->streamDelegate.SetReceivedCallback(
        [this](net::QuartcStreamInterface * s, const char * data, size_t len)
        {
          return_if(!this->stream.load() || this->stream.load() != s);

          Log::Debug("Quic stream received data: s=0x%x len=%llu", (intptr_t)s, (long long unsigned)len);

          if (this->onReceived)
          {
#ifdef DEBUG
            this->totalBytesReceived += len;
            ++this->readCount;
            Log::Debug("QuicConnction: totalBytesSent=%llu totalBytesReceived=%llu writeCount=%llu readCount=%llu",
              (long long unsigned)this->totalBytesSent,
              (long long unsigned)this->totalBytesReceived,
              (long long unsigned)this->writeCount,
              (long long unsigned)this->readCount);
#endif          
            this->onReceived(this, data, len);
          }
        }
      );

      this->streamDelegate.SetCloseCallback(
        [this](net::QuartcStreamInterface * s)
        {
          Log::Debug("Quic stream closed: s=0x%x", (intptr_t)s);

          this->closed = true;

          if (this->onReceived)
          {
            this->onReceived(this, nullptr, 0);
          }
        }
      );

      this->streamDelegate.SetCanWriteCallback(
        [this](net::QuartcStreamInterface * s)
        {
          Log::Debug("Quic stream can write: s=%p buffered=%llu written=%llu", s, 
            (long long unsigned)(((net::QuartcStream *)s)->BufferedDataBytes()),
            (long long unsigned)(((net::QuartcStream *)s)->stream_bytes_written())
          );

          if (this->closing)
          {
            Log::Debug("OnCanWrite: closing");
          }
        }
      );

      stream->SetDelegate(&this->streamDelegate);
    }


    QuicConnection::~QuicConnection()
    {
      Log::Debug("QuicConnection::~QuicConnection: this=%p", this);
      this->Close();
    }


    bool QuicConnection::Send(Buffer buffer)
    {
      return_false_if(!this->taskRunner);

      return this->taskRunner->PostTask(base::Bind(&QuicConnection::OnSend,
        this->weakFactory.GetWeakPtr(),
        base::Passed(&buffer)));
    }


    bool QuicConnection::SetRecvCallback(RecvCallback callback)
    {
      this->onReceived = callback;

      return true;
    }


    void QuicConnection::Shutdown()
    {
      this->closing = true;

      Buffer buffer;
      this->Send(std::move(buffer));
    }


    void QuicConnection::Close()
    {
      return_if(this->closed);

      this->closed = true;

      auto s = this->stream.exchange(nullptr);

      if (s && this->taskRunner)
      {
        Log::Verbose("Closing QUIC connection: conn=%p", this);

        s->SetDelegate(defaultStreamDelegate);

        if (!this->taskRunner->PostTask(base::Bind(&QuicConnection::OnClose, s, !this->closing)))
        {
          Log::Warning("QuicConnection::Close: Failed to post close event to task runner.");
        }
      }
    }


    void QuicConnection::OnClose(net::QuartcStream * s, bool closeStream)
    {
      return_if(!s);

      if (closeStream)
      {
        s->Close();
      }

      Log::Debug("QuicConnection::OnClose: s=%p closeStream=%d", s, closeStream ? 1 : 0);
    }


    void QuicConnection::OnSend(Buffer buffer)
    {
      return_if(this->closed);

      Log::Debug("QuicConnection::OnSend: s=0x%x buffer.size=%llu", (intptr_t)this->stream.load(), (long long unsigned)buffer.Size());

      auto s = this->stream.load();
      if (s)
      {
        if (buffer.Size() > 0)
        {
          net::QuartcStreamInterface::WriteParameters params;
          s->Write(static_cast<const char *>(buffer.Buf()), buffer.Size(), params);
#ifdef DEBUG
          this->totalBytesSent += buffer.Size();
          ++this->writeCount;
          Log::Debug("QuicConnction: totalBytesSent=%llu totalBytesReceived=%llu writeCount=%llu readCount=%llu",
            (long long unsigned)this->totalBytesSent,
            (long long unsigned)this->totalBytesReceived,
            (long long unsigned)this->writeCount,
            (long long unsigned)this->readCount);
#endif
        }
        else if (this->closing)
        {
          net::QuartcStreamInterface::WriteParameters params;
          params.fin = true;
          s->Write(nullptr, 0, params);
        }
      }
    }

  }
}
