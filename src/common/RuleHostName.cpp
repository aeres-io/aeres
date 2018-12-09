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

#include <sstream>
#include <iostream>
#include <aeres/Util.h>
#include <aeres/rule/RuleHostName.h>


namespace aeres
{
  namespace rule
  {
    RuleHostName::RuleHostName(std::string name)
    {
      std::istringstream stream(std::move(name));
      bool first = true;
      std::string token;
      while (std::getline(stream, token, '.'))
      {
        if (first && token == "*")
        {
          this->wildcard = true;
        }
        else
        {
          this->domains.emplace_back(std::move(token));
        }

        first = false;
      }
    }


    bool RuleHostName::Match(std::string target) const
    {
      RuleHostName targetHost{std::move(target)};
      return this->Match(targetHost);
    }


    bool RuleHostName::Match(const RuleHostName & target) const
    {
      return_false_if(!this->wildcard && this->domains.size() != target.domains.size());
      return_false_if(this->domains.size() > target.domains.size());

      int i = static_cast<int>(this->domains.size() - 1);
      int j = static_cast<int>(target.domains.size() - 1);
      while (i >= 0 && j >= 0)
      {
        if (this->domains[i] != target.domains[j])
        {
          return false;
        }

        --i;
        --j;
      }

      return true;
    }


    std::string RuleHostName::ToString() const
    {
      std::stringstream ss;
      if (this->wildcard)
      {
        ss << "*";
      }

      bool first = !this->wildcard;
      for (const auto & domain : this->domains)
      {
        if (!first)
        {
          ss << ".";
        }

        ss << domain;

        first = false;
      }

      return ss.str();
    }


    bool RuleHostName::IsEmpty() const
    {
      return !this->wildcard && this->domains.empty();
    }
  }
}