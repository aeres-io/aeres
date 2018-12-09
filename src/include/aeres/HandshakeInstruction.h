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

#include <string>
#include <stdint.h>
#include <aeres/IInputStream.h>
#include <aeres/IOutputStream.h>

namespace aeres
{
  class HandshakeInstruction
  {
  public:

    enum class Protocol
    {
      TCP,
      UDP,
      __MAX__
    };

  public:

    const std::string & HostName() const     { return this->hostname; }

    uint16_t Port() const                    { return this->port; }

    Protocol GetProtocol() const             { return this->protocol; }

    const std::string & Data() const         { return this->data; }

    void SetHostName(std::string val)        { this->hostname = std::move(val); }

    void SetPort(uint16_t val)               { this->port = val; }

    void SetProtocol(Protocol val)           { this->protocol = val; }

    void SetData(std::string val)            { this->data = std::move(val); }

    bool Serialize(IOutputStream & stream) const;

    bool Deserialize(IInputStream & stream);

  private:

    std::string hostname;

    uint16_t port = 0;

    Protocol protocol = Protocol::TCP;

    std::string data;
  };
}