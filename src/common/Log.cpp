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

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <algorithm>
#include <aeres/Log.h>


namespace aeres
{
#ifdef DEBUG
  LogLevel Log::defaultLevel = LogLevel::DBG;
#else
  LogLevel Log::defaultLevel = LogLevel::WARNING;
#endif

  FILE * Log::fp = nullptr;

  LogLevel Log::GetDefaultLevel()
  {
    return defaultLevel;
  }


  void Log::SetDefaultLevel(LogLevel val)
  {
    defaultLevel = val;
  }


  FILE * Log::GetTarget()
  {
    return fp;
  }


  void Log::SetTarget(FILE * val)
  {
    fp = val;
  }


  void Log::Write(LogLevel level, const char * format, ...)
  {
    va_list argptr;
    va_start(argptr, format);
    Write(level, format, argptr);
    va_end(argptr);
  }

#define LOG_IMPL(__func, __level) \
  void Log::__func(const char * format, ...) \
  { \
    va_list argptr; \
    va_start(argptr, format); \
    Write(__level, format, argptr); \
    va_end(argptr); \
  }

  LOG_IMPL(Debug, LogLevel::DBG);
  LOG_IMPL(Verbose, LogLevel::VERBOSE);
  LOG_IMPL(Information, LogLevel::INFOMATION);
  LOG_IMPL(Warning, LogLevel::WARNING);
  LOG_IMPL(Error, LogLevel::ERROR);
  LOG_IMPL(Critical, LogLevel::CRITICAL);


  static const char * logLevelToString(LogLevel level)
  {
    static const char * strs[] = {
      "DEBUG",
      "VERBO",
      "INFO",
      "WARN",
      "ERROR",
      "CRITI"
    };

    return strs[static_cast<int>(level)];
  }

  void Log::Write(LogLevel level, const char * format, va_list argptr)
  {
    if (!fp || level < defaultLevel)
    {
      return;
    }

    char buf[BUFSIZ];
    vsnprintf(buf, sizeof(buf) - 1, format, argptr);

    time_t ts;
    time(&ts);

    char timestr[128];
    struct tm * timeinfo = localtime(&ts);
    strftime(timestr, sizeof(timestr), "%m-%d %T", timeinfo);

    fprintf(fp, "[%s][%-5s] %s\n", timestr, logLevelToString(level), buf);
    fflush(fp);
  }


  LogLevel Log::LogLevelFromInt(int value)
  {
    value = std::max<int>(value, static_cast<int>(LogLevel::DBG));
    value = std::min<int>(value, static_cast<int>(LogLevel::CRITICAL));
    return static_cast<LogLevel>(value);
  }
}
