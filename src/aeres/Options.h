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
#include <vector>

#include <aeres/Log.h>

namespace aeres
{
  namespace Action
  {
    enum T
    {
      None = 0,
      Add = 1,
      Update = 2,
      Show = 3,
      List = 4,
      Remove = 5,
      RemoveAll = 6,
      Get = 7,
      GetAll = 8,
      Set = 9,
      SetRules = 10,
      Del = 11,
      Unknown = 12
    };

    const char * ToString(T value);
  }

  namespace Command
  {
    enum T
    {
      None = 0,
      Application = 1,
      Endpoint = 2,
      Rule = 3,
      Listen = 4,
      Tunnel = 5,
      SaveConfig = 6,
      Unknown = 7
    };

    const char * ToString(T value);
  }

  class Options
  {
  public:
    static Command::T command;
    static std::string username;
    static std::string password;
    static std::string host;
    static std::string portal;
    static uint16_t port;
    static std::string cfgFile;
    static std::string logFile;
    static LogLevel logLevel;
    static Action::T action;
    static std::string applicationId;
    static std::string endpointId;
    static std::string ruleId;
    static std::string key;
    static std::vector<std::string> args;
    static std::string arg1;
    static std::string arg2;
    static bool daemon;
    static std::string rulesFile;

    static bool Usage(const char * message = nullptr, ...);
    static bool Init(std::vector<std::string> args);
    static bool Validate();
  };
}
