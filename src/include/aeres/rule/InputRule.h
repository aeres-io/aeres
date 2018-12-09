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

#include <aeres/rule/RuleBase.h>

namespace aeres
{
  namespace rule
  {
    class InputRule : public RuleBase
    {
    public:

      InputRule();

      const RuleHostName & HostName() const     { return this->hostname; }

      uint16_t SrcPortStart() const             { return this->srcPortStart; }

      uint16_t SrcPortEnd() const               { return this->srcPortEnd; }

      uint8_t Protocol() const                  { return this->protocol; }

      void SetHostName(RuleHostName val)        { this->hostname = std::move(val); }

      void SetProtocol(uint8_t val)             { this->protocol = val; }

      bool SetPorts(uint16_t srcStart, uint16_t srcEnd);

    public:

      bool Resolve(const Host & input, bool tcp, Host & output) override;

      std::string ToString() const override;

      bool FromString(const std::string & str) override;

    private:

      RuleHostName hostname;

      uint16_t srcPortStart = 1;

      uint16_t srcPortEnd = 65535;

      uint8_t protocol = 0;
    };
  }
}