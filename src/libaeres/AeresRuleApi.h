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
#include <stdlib.h>
#include <memory>
#include <string>
#include "AeresObject.h"
#include "AsyncResult.h"

#ifdef _WIN32
#include <basetsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace aeres
{
  class AeresRuleApi : public AeresObject
  {
  public:

    AeresRuleApi(const char * base, const char * name, const char * path, const char * type);

    AsyncResultPtr<Json::Value> GetAction();
    AsyncResultPtr<Json::Value> SetAction(std::string & value);
    AsyncResultPtr<Json::Value> GetDomain();
    AsyncResultPtr<Json::Value> SetDomain(std::string & value);
    AsyncResultPtr<Json::Value> GetPort();
    AsyncResultPtr<Json::Value> SetPort(std::string & value);
    AsyncResultPtr<Json::Value> GetProtocol();
    AsyncResultPtr<Json::Value> SetProtocol(std::string & value);
    AsyncResultPtr<Json::Value> GetProperties();
    AsyncResultPtr<bool> Delete();
    AsyncResultPtr<Json::Value> Update(const std::string & action, const std::string & domain, const std::string & port, const std::string & protocol);
  };
}
