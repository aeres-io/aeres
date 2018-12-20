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
#include "AeresEndpointApi.h"

namespace aeres
{
  AERES_TYPE_REG(Endpoint, AeresEndpointApi);


  AeresEndpointApi::AeresEndpointApi(const char * base, const char * name, const char * path, const char * type)
    : AeresObject(base, name, path, type)
  {
  }

  AsyncResultPtr<Json::Value> AeresEndpointApi::GetProperties()
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


  AsyncResultPtr<Json::Value> AeresEndpointApi::GetDescription()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetDescription", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::SetDescription(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetDescription", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::GetDomains()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetDomains", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::GetPorts()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetPorts", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::SetDomains(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetDomains", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::SetPorts(std::string & value)
  {
    AeresObject::CArgs args;
    args["value"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetPorts", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<bool> AeresEndpointApi::Delete()
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

  AsyncResultPtr<Json::Value> AeresEndpointApi::GetRules()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetRules", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(error || !response.isArray() ? Json::Value() : std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

  AsyncResultPtr<Json::Value> AeresEndpointApi::NewRule(const std::string & action, const std::string & domain, const std::string & port, const std::string & protocol)
  {
    AeresObject::CArgs args;
    Json::Value value;
    value["Action"] = action;
    value["Domain"] = domain;
    value["Port"] = port;
    value["Protocol"] = protocol;

    args["spec"] = value;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("NewRule", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(error || !response.isObject() ? Json::Value() : std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresEndpointApi::SetRules(const std::string & domains, const std::string & ports)
  {
    AeresObject::CArgs args;

    args["domains"] = domains;
    args["ports"] = ports;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("SetRules", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        result->Complete(std::move(response));
      }
    );

    return rtn ? result : nullptr;
  }

}
