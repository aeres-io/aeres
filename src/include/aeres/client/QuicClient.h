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

#include <stdint.h>
#include <string>
#include <thread>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>

#include <aeres/rule/RuleConfig.h>
#include <aeres/ConnectionFactory.h>
#include <aeres/client/QuicClientSession.h>

namespace aeres
{
  namespace client
  {
    class QuicClient
    {
    public:

      explicit QuicClient(std::string id, std::string host, uint16_t port, rule::RuleConfig * ruleConfig);

      ~QuicClient();

      bool Start();

      void Stop();

      void WaitForExit();

    private:

      void ThreadProc();

    private:

      std::string id;

      std::string host;

      uint16_t port;

      std::thread thread;

      std::atomic<bool> active{false};

      std::map<QuicClientSession *, std::unique_ptr<QuicClientSession>> sessions;

      std::unique_ptr<ConnectionFactory> factory;

      std::mutex sessionMutex;

      rule::RuleConfig * ruleConfig;
    };
  }
}
