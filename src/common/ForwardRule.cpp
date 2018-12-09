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
#include <aeres/rule/ForwardRule.h>


namespace aeres
{
  namespace rule
  {
    ForwardRule::ForwardRule()
      : RuleBase(RuleType::FORWARD)
    {
    }


    bool ForwardRule::SetPorts(uint16_t srcStart, uint16_t srcEnd, uint16_t dest)
    {
      return_false_if(srcStart > srcEnd);

      // dest must be 0 (original port) if src is a range
      return_false_if(srcStart != srcEnd && dest != 0);

      // if input is all ports, then all three should be set as all ports (0)
      return_false_if((srcStart == 0 || srcEnd == 0) && (srcStart != 0 || srcEnd != 0 || dest != 0));

      this->srcPortStart = srcStart;
      this->srcPortEnd = srcEnd;
      this->destPort = dest;

      return true;
    }


    bool ForwardRule::Resolve(const Host & input, bool tcp, Host & output)
    {
      return_false_if(input.port == 0);
      return_false_if(this->srcPortStart != 0 && (input.port < this->srcPortStart || input.port > this->srcPortEnd));
      return_false_if((tcp && !(this->Protocol() & RuleProtocol::TCP)) || (!tcp && !(this->Protocol() & RuleProtocol::UDP)));
      return_false_if(input.hostname.IsEmpty() || this->srcHost.IsEmpty() || input.hostname.IsWildcard() || this->destHost.IsWildcard());
      return_false_if(!this->srcHost.Match(input.hostname));

      output.hostname = this->destHost.IsEmpty() ? input.hostname : this->destHost;
      output.port = this->destPort == 0 ? input.port : this->destPort;
      return true;
    }


    std::string ForwardRule::ToString() const
    {
      std::stringstream ss;
      ss << "FORWARD";

      if (this->Protocol() & RuleProtocol::TCP)
      {
        ss << " -p tcp";
      }

      if (this->Protocol() & RuleProtocol::UDP)
      {
        ss << " -p udp";
      }

      auto name = this->srcHost.ToString();
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

      name = this->destHost.ToString();
      if (!name.empty())
      {
        ss << " --to-destination " << name;
        if (this->destPort != 0)
        {
          ss << ":" << this->destPort;
        }
      }

      return ss.str();
    }


    bool ForwardRule::FromString(const std::string & str)
    {
      return_false_if(str.size() < sizeof("FORWARD ") - 1);
      return_false_if(str.compare(0, sizeof("FORWARD ") - 1, "FORWARD ") != 0);

      std::istringstream stream(str.substr(sizeof("FORWARD ") - 1));
      std::string token;
      uint16_t start, end, dest;
      start = end = dest = 0;
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
          this->srcHost = RuleHostName(token);
        }
        else if (token == "--dport")
        {
          return_false_if(!std::getline(stream, token, ' '));
          size_t delim = token.find(":");
          if (delim != std::string::npos)
          {
            return_false_if(delim == token.size() - 1);
            start = static_cast<uint16_t>(atoi(token.substr(0, delim).c_str()));
            end = static_cast<uint16_t>(atoi(token.substr(delim + 1).c_str()));
          }
          else
          {
            start = end = static_cast<uint16_t>(atoi(token.c_str()));
          }
        }
        else if (token == "--to-destination")
        {
          return_false_if(!std::getline(stream, token, ' '));
          size_t delim = token.find(":");
          if (delim != std::string::npos)
          {
            return_false_if(delim == token.size() - 1);
            this->destHost = RuleHostName(token.substr(0, delim));
            dest = static_cast<uint16_t>(atoi(token.substr(delim + 1).c_str()));
          }
          else
          {
            this->destHost = RuleHostName(token);
          }
        }
        else if (!token.empty() && token[0] == '#')
        {
          break;
        }
      }

      return_false_if(!this->SetPorts(start, end, dest));
      return true;
    }
  }
}