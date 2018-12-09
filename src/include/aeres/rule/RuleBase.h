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

#include <memory>
#include <aeres/rule/RuleHostName.h>
#include <aeres/rule/RuleType.h>

namespace aeres
{
  namespace rule
  {
    class RuleBase
    {
    public:

      struct Host
      {
        RuleHostName hostname;
        uint16_t port = 0;
      };

      enum RuleProtocol
      {
        TCP = 1,
        UDP = 2
      };


    public:

      explicit RuleBase(RuleType type)
        : type(type)
      {
      }

      virtual ~RuleBase() = default;

      RuleType Type() const     { return this->type; }

      virtual bool Resolve(const Host & input, bool tcp, Host & output) = 0;

      virtual std::string ToString() const = 0;

      virtual bool FromString(const std::string & str) = 0;

    public:

      static std::unique_ptr<RuleBase> Create(RuleType type);

      static std::unique_ptr<RuleBase> Create(const std::string line);

    private:

      RuleType type;
    };
  }
}