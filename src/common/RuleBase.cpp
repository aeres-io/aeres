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

#include <aeres/Util.h>
#include <aeres/rule/InputRule.h>
#include <aeres/rule/ForwardRule.h>


namespace aeres
{
  namespace rule
  {
    std::unique_ptr<RuleBase> RuleBase::Create(RuleType type)
    {
      switch (type)
      {
        case RuleType::INPUT:
          return std::unique_ptr<RuleBase>(new InputRule());

        case RuleType::FORWARD:
          return std::unique_ptr<RuleBase>(new ForwardRule());

        default:
          return nullptr;
      }
    }


    std::unique_ptr<RuleBase> RuleBase::Create(const std::string line)
    {
      if (line.empty())
      {
        return nullptr;
      }

      size_t pos = line.find_first_not_of(" \t\r\n");
      if (pos == std::string::npos || line[pos] == '#')
      {
        return nullptr;
      }
      else
      {
        size_t end = line.find_first_of(" \t\r\n", pos);
        end = end == std::string::npos ? line.size() : end;
        std::string token = line.substr(pos, end - pos);
        RuleType type = ToRuleType(token.c_str());
        auto rule = RuleBase::Create(type);
        return_val_if(nullptr, !rule || !rule->FromString(line.substr(pos)));
        return std::move(rule);
      }
    }
  }
}