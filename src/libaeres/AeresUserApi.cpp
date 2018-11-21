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
#include "AeresUserApi.h"

namespace aeres
{
  AERES_TYPE_REG(User, AeresUserApi);


  AeresUserApi::AeresUserApi(const char * base, const char * name, const char * path, const char * type)
    : AeresObject(base, name, path, type)
  {
  }


  AsyncResultPtr<Json::Value> AeresUserApi::GetApplications()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetApplications", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        if (error || !response.isArray())
        {
          result->Complete(Json::Value());
        }
        else
        {
          result->Complete(std::move(response));
        }
      }
    );

    return rtn ? result : nullptr;
  }
}
