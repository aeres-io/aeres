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
#include <json/json.h>
#include "AeresTypes.h"
#include "Base64Encoder.h"
#include "AeresSystemSecurityApi.h"

namespace aeres
{
  AERES_TYPE_REG(SystemSecurity, AeresSystemSecurityApi);


  AeresSystemSecurityApi::AeresSystemSecurityApi(const char * base, const char * name, const char * path, const char * type)
    : AeresObject(base, name, path, type)
  {
  }


  AsyncResultPtr<std::string> AeresSystemSecurityApi::LoginPassword(const std::string & username, const std::string & password)
  {
    AeresObject::CArgs args;
    args["username"] = Json::Value(username);
    args["password"] = Json::Value(password);

    auto result = std::make_shared<AsyncResult<std::string>>();

    bool rtn = this->Call("LoginPassword", args,
      [result](std::string && data, bool error)
      {
        result->SetError(error || data.find("Error") != std::string::npos);
        if (error)
        {
          result->Complete(std::string());
        }
        else
        {
          result->Complete(std::move(data));
        }
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<AeresSystemSecurityApi::LoginEndpointResult> AeresSystemSecurityApi::LoginEndpoint(const std::string & appId, const std::string & endpointId)
  {
    AeresObject::CArgs args;
    args["appId"] = Json::Value(appId);
    args["endpointId"] = Json::Value(endpointId);

    auto result = std::make_shared<AsyncResult<LoginEndpointResult>>();

    bool rtn = this->Call("LoginEndpoint", args,
      [result](Json::Value & data, bool error)
      {
        result->SetError(error);

        if (error || !data.isObject() || data.isNull())
        {
          result->Complete(LoginEndpointResult());
        }
        else
        {
          LoginEndpointResult obj;
          obj.et = data["et"].isString() ? data["et"].asString() : std::string();
          if (data["tunnelRules"].isArray())
          {
            for (size_t i = 0; i < data["tunnelRules"].size(); ++i)
            {
              const auto & line = data["tunnelRules"][i];
              if (line.isString())
              {
                obj.tunnelRules.emplace_back(line.asString());
              }
            }
          }
          result->Complete(std::move(obj));
        }
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<Json::Value> AeresSystemSecurityApi::VerifyEndpointToken(const std::string & et)
  {
    AeresObject::CArgs args;
    args["et"] = Json::Value(et);

    auto result = std::make_shared<AsyncResult<Json::Value>>();

    bool rtn = this->Call("VerifyEndpointToken", args,
      [result](Json::Value & data, bool error)
      {
        result->SetError(error);

        if (error)
        {
          result->Complete(std::string());
        }
        else
        {
          result->Complete(std::move(data));
        }
      }
    );

    return rtn ? result : nullptr;
  }


  AsyncResultPtr<AeresSystemSecurityApi::LoginSocksResult> AeresSystemSecurityApi::LoginSocks(const std::string & username, const std::string & password)
  {
    AeresObject::CArgs args;
    args["username"] = username;
    args["password"] = password;

    auto result = std::make_shared<AsyncResult<LoginSocksResult>>();

    bool rtn = this->Call("LoginSocks", args,
      [result](Json::Value & data, bool error)
      {
        result->SetError(error);

        if (error || !data.isObject() || data.isNull())
        {
          result->Complete(LoginSocksResult());
        }
        else
        {
          LoginSocksResult obj;
          obj.endpoint = data["Endpoint"].isString() ? data["Endpoint"].asString() : std::string();
          obj.mode = data["Mode"].isString() ? data["Mode"].asString() : std::string();
          result->Complete(std::move(obj));
        }
      }
    );

    return rtn ? result : nullptr;
  }

}
