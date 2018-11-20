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

#include "Options.h"
#include "Config.h"

#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>


namespace aeres
{

namespace Action
{
  const char * ToString(T value)
  {
    switch (value)
    {
      case None: return "None";
      case Add: return "Add";
      case Update: return "Update";
      case Show: return "Show";
      case List: return "List";
      case Remove: return "Remove";
      case RemoveAll: return "RemoveAll";
      case Get: return "Get";
      case GetAll: return "GetAll";
      case Set: return "Set";
      case Del: return "Del";
      default:
      case Unknown: return "Unknown";
    }
  }
}

namespace Command
{
  const char * ToString(T value)
  {
    switch (value)
    {
      case None: return "None";
      case Application: return "Application";
      case Endpoint: return "Endpoint";
      case Rule: return "Rule";
      case Listen: return "Listen";
      case Tunnel: return "Tunnel";
      default:
      case Unknown: return "Unknown";
    }
  }
}

Command::T Options::command = Command::None;
std::string Options::username;
std::string Options::password;
std::string Options::host;
uint16_t Options::port;
std::string Options::cfgFile;
std::string Options::logFile;
LogLevel Options::logLevel =
#ifndef DEBUG
  LogLevel::INFOMATION;
#else
  LogLevel::DBG;
#endif
Action::T Options::action = Action::None;
std::string Options::applicationId;
std::string Options::endpointId;
std::string Options::ruleId;
std::string Options::name;
std::string Options::value;
std::string Options::key;
bool Options::daemon;

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

  switch (Options::command)
  {
    case Command::Application:
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
      break;
    case Command::Endpoint:
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
      break;
    case Command::Rule:
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
      break;
    case Command::Listen:
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
      break;
    case Command::Tunnel:
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
      printf("  -k,--key <key>                  Client key (optional)\n");
      printf("\n");
    default:
    case Command::None:
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
      printf("  aeres saveconfig               Save options to config\n");
      printf("\n");
      printf("Options:\n");
      printf("\n");
      printf("  -u,--username <user>           Username / Email address\n");
      printf("  -p,--password <password>       Password\n");
      printf("  -h,--host <host>[:port]        Aeres host server\n");
      printf("  -c,--config <config-file>      Config path (/etc/aeres/aeres.conf)\n");
      printf("  -l,--log <log-file>            Log path (/var/log/aeres.log)\n");
      printf("  -?,--help                      Help\n");
      printf("\n");
      break;
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

  int i = 1;

  if (argc >= 2 && argv[1][0] != '-')
  {
    i++;
    switch (argv[1][0])
    {
      case 'a': case 'A':
        command = Command::Application;
        break;
      case 'e': case 'E':
        command = Command::Endpoint;
        break;
      case 'r': case 'R':
        command = Command::Rule;
        break;
      case 'l': case 'L':
        command = Command::Listen;
        break;
      case 't': case 'T':
        command = Command::Tunnel;
        break;
      case 's': case 'S':
        command = Command::SaveConfig;
        break;
      default:
        command = Command::Unknown;
        break;
    }
  }

  for (; i < argc; ++i)
  {
    if (strcmp(argv[i], "--add") == 0)
    {
      action = Action::Add;
    }
    else if (strcmp(argv[i], "--update") == 0)
    {
      action = Action::Update;
    }
    else if (strcmp(argv[i], "--show") == 0)
    {
      action = Action::Show;
    }
    else if (strcmp(argv[i], "--list") == 0)
    {
      action = Action::List;
    }
    else if (strcmp(argv[i], "--remove") == 0)
    {
      action = Action::Remove;
    }
    else if (strcmp(argv[i], "--remove-all") == 0)
    {
      action = Action::RemoveAll;
    }
    else if (strcmp(argv[i], "--get") == 0)
    {
      action = Action::Get;
    }
    else if (strcmp(argv[i], "--get-all") == 0)
    {
      action = Action::GetAll;
    }
    else if (strcmp(argv[i], "--set") == 0)
    {
      action = Action::Set;
    }
    else if (strcmp(argv[i], "--del") == 0)
    {
      action = Action::Del;
    }
    else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--application") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      applicationId = argv[i];
    }
    else if (strcmp(argv[i], "-e") == 0 || strcmp(argv[i], "--endpoint") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      endpointId = argv[i];
    }
    else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--rule") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      ruleId = argv[i];
    }
    else if (strcmp(argv[i], "-u") == 0 || strcmp(argv[i], "--username") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      username = argv[i];
    }
    else if (strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--password") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      password = argv[i];
    }
    else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--host") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      host = argv[i];
    }
    else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--daemon") == 0)
    {
      daemon = true;
    }
    else if (strcmp(argv[i], "-k") == 0 || strcmp(argv[i], "--key") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      key = argv[i];
    }
    else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--config") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      cfgFile = argv[i];
    }
    else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--log") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      logFile = argv[i];
    }
    else if (strcmp(argv[i], "--log-level") == 0)
    {
      assert_argument_index(++i, argv[i-1]);
      logLevel = Log::LogLevelFromInt(atoi(argv[i]));
    }
    else if (strcmp(argv[i], "-?") == 0 || strcmp(argv[i], "--help") == 0)
    {
      Usage();
    }
    else
    {
      Usage("Error: unknown argument '%s'.\n", argv[i]);
    }
  }

  return Validate();
}

bool Options::Validate()
{
  if (cfgFile.size() == 0)
  {
    const char *homedir;
    if ((homedir = getenv("HOME")) == NULL)
    {
      homedir = getpwuid(getuid())->pw_dir;
    }
    if (homedir != NULL)
    {
      cfgFile = homedir;
      cfgFile.append("/");
    }
    cfgFile.append(".aeres.conf");
  }
  Config config(cfgFile);
  if (config.Load())
  {
    if (Options::username.size() == 0 && config.Root().isMember("username"))
    {
      Options::username = config.Root()["username"].asString();
    }
    if (Options::password.size() == 0 && config.Root().isMember("password"))
    {
      Options::password = config.Root()["password"].asString();
    }
    if (Options::applicationId.size() == 0 && config.Root().isMember("application"))
    {
      Options::applicationId = config.Root()["application"].asString();
    }
    if (Options::endpointId.size() == 0 && config.Root().isMember("endpoint"))
    {
      Options::endpointId = config.Root()["endpoint"].asString();
    }
    if (Options::logFile.size() == 0 && config.Root().isMember("log"))
    {
      Options::logFile = config.Root()["log"].asString();
    }
    if (Options::host.size() == 0 && config.Root().isMember("host"))
    {
      Options::host = config.Root()["host"].asString();
    }
    if(Options::port == 0 && config.Root().isMember("port"))
    {
      Options::port = config.Root()["port"].asUInt();
    }
  }

  if (Options::command == Command::SaveConfig)
  {
    if (Options::username.size() > 0)
    {
      config.Root()["username"] = Options::username;
    }
    if (Options::password.size() > 0)
    {
      config.Root()["password"] = Options::password;
    }
    if (Options::applicationId.size() > 0)
    {
      config.Root()["application"] = Options::applicationId;
    }
    if (Options::endpointId.size() > 0)
    {
      config.Root()["endpoint"] = Options::endpointId;
    }
    if (Options::logFile.size() > 0)
    {
      config.Root()["log"] = Options::logFile;
    }
    if (Options::host.size() > 0)
    {
      config.Root()["host"] = Options::host;
    }
    if (Options::port > 0)
    {
      config.Root()["port"] = (uint64_t)Options::port;
    }
    config.Save();
  }

  if (Options::logFile.size() == 0)
  {
    logFile = "/var/log/aeres.log";
  }
  if (Options::host.size() == 0)
  {
    Options::host = "aereslab.com";
  }
  if (Options::port == 0)
  {
    Options::port = 7900;
  }

  switch (Options::command)
  {
    case Command::None:
      return true;
    case Command::Application:
      if (Options::username.size() == 0)
      {
        Usage("Error: missing username\n");
      }
      if (Options::password.size() == 0)
      {
        Usage("Error: missing password\n");
      }
      switch (Options::action)
      {
        case Action::None:
        case Action::Unknown:
          Usage("Error: missing action\n");
        case Action::Remove:
        case Action::Update:
        case Action::Show:
        case Action::GetAll:
        case Action::Get:
        case Action::Set:
        case Action::Del:
          if (Options::applicationId.size() == 0)
          {
            Usage("Error: missing application\n");
          }
          break;
      }
      return true;
    case Command::Endpoint:
      if (Options::username.size() == 0)
      {
        Usage("Error: missing username\n");
      }
      if (Options::password.size() == 0)
      {
        Usage("Error: missing password\n");
      }
      if (Options::applicationId.size() == 0)
      {
        Usage("Error: missing application\n");
      }
      switch (Options::action)
      {
        case Action::None:
        case Action::Unknown:
          Usage("Error: missing action\n");
        case Action::Remove:
        case Action::Update:
        case Action::Show:
        case Action::GetAll:
        case Action::Get:
        case Action::Set:
        case Action::Del:
          if (Options::endpointId.size() == 0)
          {
            Usage("Error: missing endpoint\n");
          }
          break;
      }
      return true;
    case Command::Rule:
      if (Options::username.size() == 0)
      {
        Usage("Error: missing username\n");
      }
      if (Options::password.size() == 0)
      {
        Usage("Error: missing password\n");
      }
      if (Options::applicationId.size() == 0)
      {
        Usage("Error: missing application\n");
      }
      if (Options::endpointId.size() == 0)
      {
        Usage("Error: missing endpoint\n");
      }
      switch (Options::action)
      {
        case Action::None:
        case Action::Unknown:
          Usage("Error: missing action\n");
        case Action::Remove:
        case Action::Update:
        case Action::Show:
        case Action::GetAll:
        case Action::Get:
        case Action::Set:
        case Action::Del:
          if (Options::ruleId.size() == 0)
          {
            Usage("Error: missing rule\n");
          }
          break;
      }
      return true;
    case Command::Listen:
      if (Options::applicationId.size() == 0)
      {
        Usage("Error: missing application\n");
      }
      if (Options::endpointId.size() == 0)
      {
        Usage("Error: missing endpoint\n");
      }
      return true;
    case Command::Tunnel:
      if (Options::endpointId.size() == 0)
      {
        Usage("Error: missing endpoint\n");
      }
      return true;
  }
  return true;
}

}
