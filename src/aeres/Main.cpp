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


#include <stdio.h>
#include <iostream>
#include <thread>
#include <chrono>

#include <openssl/ssl.h>

#include <aeres/Log.h>
#include "AeresSession.h"
#include "AeresApplicationCli.h"
#include "AeresEndpointCli.h"
#include "AeresRuleCli.h"
#include "AeresListener.h"
#include "AeresTunnel.h"

#include "Options.h"

using namespace aeres;

int main(int argc, const char ** argv)
{
  SSL_library_init();

  int res = 0;

  Options::Init(argc, argv);

  FILE * log = stdout;

  if (!Options::logFile.empty())
  {
    log = fopen(Options::logFile.c_str(), "a");
    if (!log)
    {
      log = stdout;
    }
  }

  aeres::Log::SetTarget(log);
  aeres::Log::SetDefaultLevel(Options::logLevel);

  auto session = AeresSession::CreateSession(Options::host.c_str());

  switch (Options::command)
  {
    case Command::None:
    {
      printf("TODO: REPL environment\n");
      break;
    }
    case Command::Application:
    {
      AeresApplicationCli application(session);
      res = application.Process();
      break;
    }
    case Command::Endpoint:
    {
      AeresEndpointCli endpoint(session);
      res = endpoint.Process();
      break;
    }
    case Command::Rule:
    {
      AeresRuleCli rule(session, Options::endpointId);
      res = rule.Process();
      break;
    }
    case Command::Listen:
    {
      AeresListener listener(Options::applicationId, Options::endpointId);
      res = listener.Process();
      break;
    }
    case Command::Tunnel:
    {
      AeresTunnel tunnel(Options::endpointId);
      res = tunnel.Process();
      break;
    }
    default:
    case Command::SaveConfig:
    {
      break;
    }
  }

  if (log && log != stdout)
  {
    fclose(log);
  }

  AeresSession::Stop();
  return res;
}
