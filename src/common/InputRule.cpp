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
#include <aeres/Util.h>
#include <aeres/rule/InputRule.h>


namespace aeres
{
  namespace rule
  {
    InputRule::InputRule()
      : RuleBase(RuleType::INPUT)
    {
    }


    bool InputRule::SetPorts(uint16_t srcStart, uint16_t srcEnd)
    {
      return_false_if(srcStart > srcEnd);
      return_false_if((srcStart == 0 || srcEnd == 0) && (srcStart != srcEnd))

      this->srcPortStart = srcStart;
      this->srcPortEnd = srcEnd;

      return true;
    }


    bool InputRule::Resolve(const Host & input, bool tcp, Host & output)
    {
      return_false_if(input.port == 0);
      return_false_if(this->srcPortStart != 0 && (input.port < this->srcPortStart || input.port > this->srcPortEnd));
      return_false_if((tcp && !(this->Protocol() & RuleProtocol::TCP)) || (!tcp && !(this->Protocol() & RuleProtocol::UDP)));
      return_false_if(input.hostname.IsEmpty() || this->hostname.IsEmpty() || input.hostname.IsWildcard());
      return_false_if(!this->hostname.Match(input.hostname));

      output.hostname = RuleHostName("localhost");
      output.port = input.port;
      return true;
    }


    std::string InputRule::ToString() const
    {
      std::stringstream ss;
      ss << "INPUT";

      if (this->Protocol() & RuleProtocol::TCP)
      {
        ss << " -p tcp";
      }

      if (this->Protocol() & RuleProtocol::UDP)
      {
        ss << " -p udp";
      }

      auto name = this->hostname.ToString();
      if (!name.empty())
      {
        ss << " -i " << name;
      }

      if (this->srcPortStart != 0)
      {
        ss << " --dport " << this->srcPortStart;
        if (this->srcPortEnd != this->srcPortStart)
        {
          ss << ":" << this->srcPortEnd;
        }
      }

      return ss.str();
    }


    bool InputRule::FromString(const std::string & str)
    {
      return_false_if(str.size() < sizeof("INPUT ") - 1);
      return_false_if(str.compare(0, sizeof("INPUT ") - 1, "INPUT ") != 0);

      std::istringstream stream(str.substr(sizeof("INPUT ") - 1));
      std::string token;
      while (std::getline(stream, token, ' '))
      {
        if (token == "-p")
        {
          return_false_if(!std::getline(stream, token, ' '));
          if (token == "tcp")
          {
            this->protocol |= RuleProtocol::TCP;
          }
          else if (token == "udp")
          {
            this->protocol |= RuleProtocol::UDP;
          }
        }
        else if (token == "-i")
        {
          return_false_if(!std::getline(stream, token, ' '));
          this->hostname = RuleHostName(token);
        }
        else if (token == "--dport")
        {
          return_false_if(!std::getline(stream, token, ' '));
          size_t delim = token.find(":");
          if (delim != std::string::npos)
          {
            return_false_if(delim == token.size() - 1);
            std::string start, end;
            start = token.substr(0, delim);
            end = token.substr(delim + 1);

            return_false_if(!this->SetPorts(static_cast<uint16_t>(atoi(start.c_str())), static_cast<uint16_t>(atoi(end.c_str()))));
          }
          else
          {
            uint16_t port = static_cast<uint16_t>(atoi(token.c_str()));
            return_false_if(!this->SetPorts(port, port));
          }
        }
        else if (!token.empty() && token[0] == '#')
        {
          break;
        }
      }

      return true;
    }
  }
}