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
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "Options.h"

std::string Options::appId;
std::string Options::clientId("auto");
std::string Options::host("aereslab.com");
uint16_t Options::port = 7900;
bool Options::daemon = false;

#ifndef DEBUG
aeres::LogLevel Options::logLevel = aeres::LogLevel::INFOMATION;
#else
aeres::LogLevel Options::logLevel = aeres::LogLevel::DBG;
#endif

std::string Options::logFile;


bool Options::Usage(const char * message, ...)
{
  if (message != NULL)
  {
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    printf("\n");
  }

  printf("Usage: aeres [option]\n");
  printf("\n");
  printf("Options:\n");
  printf("  -a <appid>      aeres app id (required)\n");
  printf("  -c <clientid>   aeres client id (default: auto)\n");
  printf("  -h <host>       aeres server name (default: aereslab.com)\n");
  printf("  -p <port>       aeres server port (default: 7900)\n");
  printf("  -d              start client as a daemon\n");
  printf("  -l <level>      log level (0-5. 0 for debug, 5 for critical. default:2)\n");
  printf("  -o <log_file>   log file (default: stdout)\n");
  printf("\n");
  exit(message == NULL ? 0 : 1);
}

bool Options::Init(int argc, const char **argv)
{
#define assert_argument_index(_idx, _name)    \
  if ((_idx) >= argc)                         \
  {                                           \
    Usage("Missing argument '%s'.\n", _name); \
  }

  for (int i = 1; i < argc; ++i)
  {
    if (strcmp(argv[i], "-a") == 0)
    {
      assert_argument_index(++i, "appid");
      appId = argv[i];
    }
    else if (strcmp(argv[i], "-c") == 0)
    {
      assert_argument_index(++i, "clientid");
      clientId = argv[i];
    }
    else if (strcmp(argv[i], "-h") == 0)
    {
      assert_argument_index(++i, "host");
      host = argv[i];
    }
    else if (strcmp(argv[i], "-p") == 0)
    {
      assert_argument_index(++i, "port");
      port = static_cast<uint16_t>(atoi(argv[i]));
    }
    else if (strcmp(argv[i], "-d") == 0)
    {
      daemon = true;
    }
    else if (strcmp(argv[i], "-l") == 0)
    {
      assert_argument_index(++i, "level");
      logLevel = aeres::Log::LogLevelFromInt(atoi(argv[i]));
    }
    else if (strcmp(argv[i], "-o") == 0)
    {
      assert_argument_index(++i, "log_file");
      logFile = argv[i];
    }
    else
    {
      Usage("Error: unknown argument '%s'.\n", argv[i]);
    }
  }

  if (appId.empty())
  {
    Usage("Missing argument 'appid'.\n");
  }

  if (host.empty())
  {
    Usage("Missing argument 'host'.\n");
  }

  return true;
}
