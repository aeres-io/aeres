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
#include "AeresApplicationApi.h"

namespace aeres
{
  AERES_TYPE_REG(Application, AeresApplicationApi);


  AeresApplicationApi::AeresApplicationApi(const char * base, const char * name, const char * path, const char * type)
    : AeresObject(base, name, path, type)
  {
  }


  AsyncResultPtr<Json::Value> AeresApplicationApi::GetEndpoints()
  {
    AeresObject::CArgs args;

    auto result = std::make_shared<AsyncResult<Json::Value>>();
    bool rtn = this->Call("GetEndpoints", args,
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

  AsyncResultPtr<Json::Value> AeresApplicationApi::NewEndpoint(const char * endpointId, const char * description)
  {
   AeresObject::CArgs args;

   if(endpointId && endpointId != "")
   {
     args["endpointId"] = std::string(endpointId);
   }

   if(description && description != "")
   {
     args["description"] = std::string(description);
   }

   auto result = std::make_shared<AsyncResult<Json::Value>>();
   bool rtn = this->Call("NewEndpoint", args,
     [result](Json::Value & response, bool error)
     {
       result->SetError(error);
       if (error || !response.isObject())
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


   AsyncResultPtr<Json::Value> AeresApplicationApi::NewApplication(const char * displayName)
   {
   assert(displayName);

   AeresObject::CArgs args;
   args["displayName"] = std::string(displayName);

   auto result = std::make_shared<AsyncResult<Json::Value>>();
   bool rtn = this->Call("NewApplication", args,
     [result](Json::Value & response, bool error)
     {
       result->SetError(error);
       if (error || !response.isObject())
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

  AsyncResultPtr<bool> AeresApplicationApi::Delete()
  {
    AeresObject::CArgs args;
    auto result = std::make_shared<AsyncResult<bool>>();

    bool rtn = this->Call("Delete", args,
      [result](Json::Value & response, bool error)
      {
        result->SetError(error);
        if (error || !response.isBool())
        {
          result->Complete(false);
        }
        else
        {
          result->Complete(response.asBool());
        }
      }
    );

    return rtn ? result : nullptr;
  }
}
