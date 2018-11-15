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

#include <stdint.h>
#include <list>
#include <aeres/Buffer.h>

namespace aeres
{
  class BufferStream
  {
  public:

    void WriteBuffer(const void * data, size_t len);

    void WriteBuffer(Buffer buf);

    size_t ReadBuffer(void * output, size_t len);

    size_t PeekBuffer(void * output, size_t len);

    void Clear();

    size_t Size() const   { return this->size; }

    template<typename T>
    void Write(T val)
    {
      this->WriteBuffer(&val, sizeof(T));
    }

    template<typename T>
    void Write(T * ary, size_t len)
    {
      this->WriteBuffer(ary, len * sizeof(T));
    }

    template<typename T>
    bool Read(T * val)
    {
      if (this->size < sizeof(T))
      {
        return false;
      }

      this->ReadBuffer(val, sizeof(T));
      return true;
    }

    template<typename T>
    bool Read(T * val, size_t len)
    {
      if (this->size < sizeof(T) * len)
      {
        return false;
      }

      this->ReadBuffer(val, sizeof(T) * len);
      return true;
    }

    template<typename T>
    bool Peek(T * val)
    {
      if (this->size < sizeof(T))
      {
        return false;
      }

      this->PeekBuffer(val, sizeof(T));
      return true;
    }

    template<typename T>
    bool Peek(T * val, size_t len)
    {
      if (this->size < sizeof(T) * len)
      {
        return false;
      }

      this->PeekBuffer(val, sizeof(T) * len);
      return true;
    }

  private:

    std::list<Buffer> buffers;

    size_t offset = 0;

    size_t size = 0;
  };
}
