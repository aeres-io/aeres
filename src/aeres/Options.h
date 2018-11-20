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
      Del = 10,
      Unknown = 11
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
      Unknown = 6
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
    static uint16_t port;
    static std::string logFile;
    static aeres::LogLevel logLevel;
    static aeres::Action::T action;
    static std::string applicationId;
    static std::string endpointId;
    static std::string ruleId;
    static std::string name;
    static std::string value;
    static std::string key;
    static bool daemon;

    static bool Usage(const char * message = nullptr, ...);
    static bool Init(int argc, const char ** argv);
    static bool Validate();
  };
}
