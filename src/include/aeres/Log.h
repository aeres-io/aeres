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

#include <stdio.h>
#include <stdarg.h>

namespace aeres
{
  enum class LogLevel
  {
    DBG = 0,
    VERBOSE = 1,
    INFOMATION = 2,
    WARNING = 3,
    ERROR = 4,
    CRITICAL = 5
  };

  class Log
  {
  public:

    static LogLevel GetDefaultLevel();

    static void SetDefaultLevel(LogLevel val);

    static FILE * GetTarget();

    static void SetTarget(FILE * val);

    static void Write(LogLevel level, const char * format, ...);

    static void Debug(const char * format, ...);

    static void Verbose(const char * format, ...);

    static void Information(const char * format, ...);

    static void Warning(const char * format, ...);

    static void Error(const char * format, ...);

    static void Critical(const char * format, ...);

    static LogLevel LogLevelFromInt(int value);

  private:

    static void Write(LogLevel level, const char * format, va_list argptr);

  private:

    static LogLevel defaultLevel;

    static FILE * fp;
  };
}