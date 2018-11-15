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

#include <string.h>
#include <algorithm>
#include <aeres/BufferStream.h>

namespace aeres
{
  #ifndef BUFSIZ
  #define BUFSIZ 8192
  #endif


  void BufferStream::WriteBuffer(Buffer buf)
  {
    this->size += buf.Size();
    this->buffers.emplace_back(std::move(buf));
  }


  void BufferStream::WriteBuffer(const void * data, size_t len)
  {
    auto last = this->buffers.rbegin();
    if (last == this->buffers.rend() || last->Size() + len > BUFSIZ)
    {
      this->buffers.emplace_back(Buffer());
      last = this->buffers.rbegin();
    }

    size_t pos = last->Size();
    last->Resize(pos + len);
    memcpy(static_cast<uint8_t *>(last->Buf()) + pos, data, len);

    this->size += len;
  }


  size_t BufferStream::ReadBuffer(void * output, size_t len)
  {
    uint8_t * buf = static_cast<uint8_t *>(output);
    size_t result = 0;

    while (this->size > 0 && len > 0)
    {
      auto itr = this->buffers.begin();
      if (itr == this->buffers.end())
      {
        break;
      }

      size_t bytes = std::min<size_t>(len, itr->Size() - this->offset);

      if (buf)
      {
        memcpy(buf, static_cast<uint8_t *>(itr->Buf()) + this->offset, bytes);
        buf += bytes;
      }

      result += bytes;
      this->size -= bytes;
      len -= bytes;
      this->offset += bytes;

      if (this->offset >= itr->Size())
      {
        this->offset -= itr->Size();
        this->buffers.erase(itr);
      }
    }

    return result;
  }


  size_t BufferStream::PeekBuffer(void * output, size_t len)
  {
    uint8_t * buf = static_cast<uint8_t *>(output);
    size_t result = 0;

    size_t localSize = this->size;
    size_t localOffset = this->offset;
    auto itr = this->buffers.begin();

    while (localSize > 0 && len > 0 && itr != this->buffers.end())
    {
      size_t bytes = std::min<size_t>(len, itr->Size() - localOffset);
      memcpy(buf, static_cast<uint8_t *>(itr->Buf()) + localOffset, bytes);

      result += bytes;
      buf += bytes;
      localSize -= bytes;
      len -= bytes;
      localOffset += bytes;

      if (localOffset >= itr->Size())
      {
        localOffset -= itr->Size();
        ++itr;
      }
    }

    return result;
  }


  void BufferStream::Clear()
  {
    this->buffers.clear();
    this->offset = this->size = 0;
  }
}
