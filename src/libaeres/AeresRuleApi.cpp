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

#include <assert.h>
#include <json/json.h>
#include "AeresTypes.h"
#include "Base64Encoder.h"
#include "AeresRuleApi.h"

namespace aeres
{
  AERES_TYPE_REG(Rule, AeresRuleApi);


  AeresRuleApi::AeresRuleApi(const char * base, const char * name, const char * path, const char * type)
    : AeresObject(base, name, path, type)
  {
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::GetProperties()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetProperties", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::GetAction()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetAction", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::SetAction(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetAction", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::GetDomain()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetDomain", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::SetDomain(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetDomain", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::GetPort()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetPort", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::SetPort(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetPort", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::GetProtocol()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetProtocol", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresRuleApi::SetProtocol(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetProtocol", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<bool> AeresRuleApi::Delete()
  {
    AeresObject::CArgs args;
    auto result = std::make_shared<AsyncResult<bool>>();

    bool rtn = this->Call("Delete", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(error || !response.isBool() ? false : response.asBool());
      }
    );

    return rtn ? result : nullptr;
  }

}
