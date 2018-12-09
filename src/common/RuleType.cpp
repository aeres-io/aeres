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
#include <aeres/rule/RuleType.h>

namespace aeres
{
  namespace rule
  {
    std::string ToString(RuleType type)
    {
      switch (type)
      {
        case RuleType::INPUT:
          return "INPUT";

        case RuleType::FORWARD:
          return "FORWARD";

        default:
          return std::string();
      }
    }


    RuleType ToRuleType(const char * value)
    {
      if (value)
      {
        if (strcmp(value, "INPUT") == 0)
        {
          return RuleType::INPUT;
        }
        else if (strcmp(value, "FORWARD") == 0)
        {
          return RuleType::FORWARD;
        }
      }

      return RuleType::UNDEFINED;
    }
  }
}