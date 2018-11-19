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

std::string Options::command;
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

  if (Options::command == "app")
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres app <action> [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Manage applications IDs\n");
    printf("\n");
    printf("  Note: these actions require user authentication (-u and -p)\n");
    printf("\n");
    printf("Actions:\n");
    printf("\n");
    printf("  --list                          List application IDs\n");
    printf("  --add                           Add a new application ID\n");
    printf("  --remove-all                    Remove all application IDs\n");
    printf("\n");
    printf("  Note: the following actions require an application ID (-a)\n");
    printf("\n");
    printf("  --remove                        Remove an application ID\n");
    printf("  --get-all                       Retrieves all properties\n");
    printf("  --get <property>                Retrieves a property\n");
    printf("  --set <property> <value>        Sets a property\n");
    printf("  --del <property>                Deletes a property\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -a,--application <application>  Specify the application ID\n");
    printf("\n");
  }
  else if (Options::command == "endpoint")
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres endpoint <action> [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Manages application endpoints\n");
    printf("\n");
    printf("  Note: these actions require user authentication (-u and -p) as\n");
    printf("        well as a valid application ID (-a)\n");
    printf("\n");
    printf("Actions:\n");
    printf("\n");
    printf("  --list                          List endpoints\n");
    printf("  --add                           Add a new endpoint\n");
    printf("  --remove-all                    Remove all endpoints\n");
    printf("\n");
    printf("  Note: the following actions require an endpoint ID (-e)\n");
    printf("\n");
    printf("  --remove                        Remove an endpoint\n");
    printf("  --get-all                       Retrieves all properties\n");
    printf("  --get <property>                Retrieves a property\n");
    printf("  --set <property> <value>        Sets a property\n");
    printf("  --del <property>                Deletes a property\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -e,--endpoint <endpoint>        Specify the endpoint ID\n");
    printf("\n");
    printf("Properties:\n");
    printf("\n");
    printf("  access                          public|private\n");
    printf("  password                        <string>\n");
    printf("\n");
  }
  else if (Options::command == "rule")
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres rule <action> [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Manages application endpoint rules\n");
    printf("\n");
    printf("  Note: these actions require user authentication (-u and -p) as\n");
    printf("        well as a valid application ID (-a) and endpoint ID (-e)\n");
    printf("\n");
    printf("Actions:\n");
    printf("\n");
    printf("  --list                          List rules\n");
    printf("  --add <rulespec>                Add a new rule\n");
    printf("  --remove-all                    Remove all rules\n");
    printf("\n");
    printf("  Note: the following actions require a rule ID (-r)\n");
    printf("\n");
    printf("  --remove                        Remove a rule\n");
    printf("  --update <rulespec>             Update a rule\n");
    printf("  --get-all                       Retrieves all properties\n");
    printf("  --get <property>                Retrieves a property\n");
    printf("  --set <property> <value>        Sets a property\n");
    printf("  --del <property>                Deletes a property\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -r,--rule <rule>                Specify the rule ID\n");
    printf("\n");
    printf("Properties:\n");
    printf("\n");
    printf("  action                          accept|reject|notify\n");
    printf("  url                             <dns-name>\n");
    printf("  port                            <port-range>\n");
    printf("  Protocol                        tcp|udp|both\n");
    printf("\n");
    printf("Rule Specification:\n");
    printf("\n");
    printf("  The rule is specified as a single string with the following\n");
    printf("  syntax:\n");
    printf("\n");
    printf("    <action>:<name>:<port>[:<protocol>]\n");
    printf("\n");
    printf("  Where <action> is one of:\n");
    printf("\n");
    printf("    accept\n");
    printf("    reject\n");
    printf("    notify\n");
    printf("\n");
    printf("  Where <name> is a valid DNS name (wildcards OK):\n");
    printf("\n");
    printf("    myendpoint                    eg. myendpoint.aereslab.com\n");
    printf("    www.mysite.com                eg. www.mysite.com\n");
    printf("    *.mysite.com                  eg. mysite.com or www.mysite.com\n");
    printf("\n");
    printf("  Where <port> is a port, port range, or port name:\n");
    printf("\n");
    printf("    443                           default HTTPS port\n");
    printf("    20-22                         ports 20, 21, and 22\n");
    printf("    80,443                        ports 80 and 443\n");
    printf("    HTTPS                         HTTPS port 443\n");
    printf("\n");
    printf("  Where <protocol> is one of:\n");
    printf("\n");
    printf("    udp\n");
    printf("    tcp\n");
    printf("    both\n");
    printf("\n");
  }
  else if (Options::command == "listen")
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres listen [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Creates a listener to process incoming requests\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -a,--application <application>  Application ID (required)\n");
    printf("  -e,--endpoint <endpoint>        Endpoint ID (required)\n");
    printf("  -h,--host <host>[:<port>]       Aeres Host (optional)\n");
    printf("\n");
  }
  else if (Options::command == "tunnel")
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres tunnel [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Creates a tunnel to forward outgoing requests\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -e,--endpoint <endpoint>        Endpoint ID (required)\n");
    printf("  -h,--host <host>[:<port>]       Aeres Host (optional)\n");
    printf("  -p,--password <password>        Password (optional)\n");
    printf("\n");
  }
  else
  {
    printf("Usage:\n");
    printf("\n");
    printf("  aeres <command> [options]\n");
    printf("\n");
    printf("Description:\n");
    printf("\n");
    printf("  Command line interface for Aeres\n");
    printf("\n");
    printf("Commands:\n");
    printf("\n");
    printf("  aeres                          Enter REPL environment\n");
    printf("  aeres app                      Manage applications\n");
    printf("  aeres endpoint                 Manage endpoints\n");
    printf("  aeres rule                     Manage rules\n");
    printf("  aeres listen                   Start listen service\n");
    printf("  aeres tunnel                   Start tunnel service\n");
    printf("\n");
    printf("Options:\n");
    printf("\n");
    printf("  -u,--user <user>               Username / Email address\n");
    printf("  -p,--password <password>       Password\n");
    printf("  -h,--host <host>[:port]        Aeres host server\n");
    printf("  -l,--log <log-file>            Log file location\n");
    printf("  -?,--help                      Help\n");
    printf("\n");
  }

  exit(message == NULL ? 0 : 1);
}

bool Options::Init(int argc, const char **argv)
{
#define assert_argument_index(_idx, _name)    \
  if ((_idx) >= argc)                         \
  {                                           \
    Usage("Missing argument '%s'.\n", _name); \
  }

  if (argc >= 2)
  {
    command = argv[1];
  }

  for (int i = 2; i < argc; ++i)
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
