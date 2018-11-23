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

#include "AeresSession.h"
#include "HttpRequest.h"
#include "AeresObject.h"
#include "AeresTypes.h"
#include "AsyncQueue.h"
#include "AeresSystemSecurityApi.h"

#include <sstream>

namespace aeres
{
  namespace
  {
    void OnDequeue(HttpRequest* & request, void * context)
    {
#ifdef DEBUG_API
      printf("Processing: %s\n", request->Url().c_str());
#endif
      request->Execute();
    }

    void OnDelete(HttpRequest* & request, void * context)
    {
      delete (request);
    }

    AsyncQueue<HttpRequest*> queue(OnDequeue, OnDelete, NULL);
    static bool queueStarted = false;
  }

  SharedMutex AeresSession::mutex;
  HttpConfig AeresSession::defaultConfig;
  std::map<std::string, std::shared_ptr<AeresSession> > AeresSession::sessions;

  std::shared_ptr<AeresSession> AeresSession::GetSession(const char * base)
  {
    std::string strBase(base);
    return GetSession(strBase);
  }

  std::shared_ptr<AeresSession> AeresSession::GetSession(const std::string & base)
  {
    ReadLock _(mutex);
    return sessions[base];
  }

  std::shared_ptr<AeresSession> AeresSession::CreateSession(const char * base, HttpConfig * config, bool ownConfig)
  {
    WriteLock _(mutex);
    if (!queueStarted)
    {
      queue.Start();
      queueStarted = true;
    }
    if (config == NULL)
    {
      ownConfig = false;
      config = &defaultConfig;
    }

    auto session = std::shared_ptr<AeresSession>(new AeresSession(base, config, ownConfig));
    sessions[session->Base()] = session;
    return session;
  }

  AeresSession::AeresSession(const char * base, HttpConfig * config, bool ownConfig) :
    config(config),
    base(base),
    ownConfig(ownConfig)
  {
  }

  AeresSession::~AeresSession()
  {
    if (this->ownConfig && this->config)
    {
      delete this->config;
      this->config = nullptr;
    }
  }

  void AeresSession::Stop()
  {
    if (queueStarted)
    {
      queue.Stop();
      queueStarted = false;
    }
  }

  std::string AeresSession::__EncodeArgs(AeresObject::CArgs & args)
  {
    char sep = '?';
    std::stringstream res;
    Json::FastWriter writer;
    for (AeresObject::CArgs::iterator itr = args.begin(); itr != args.end(); itr++)
    {
      res << sep << (*itr).first << '=' << HttpRequest::EncodeStr(writer.write((*itr).second).c_str());
      sep = '&';
    }
    return res.str();
  }


  HttpRequest * AeresSession::CreateRequest(std::string & path, const char * method, AeresObject::CArgs & args, Json::Value & data)
  {
    std::string pathCopy = path;
    std::stringstream ss;
    ss << base;
    ss << "/api/";
    std::size_t css = path.find("://");
    if (css == std::string::npos) { return nullptr; }
    ss << pathCopy.replace(css, 2, "", 0);
    ss << '/';
    ss << method;
    //ss << this->__EncodeArgs(args);
    std::string url = ss.str();

    for (auto it = args.begin(); it != args.end(); ++it)
    {
      data[it->first] = it->second;
    }

    return new HttpRequest(url.c_str(), config);
  }


  bool AeresSession::Call(std::string & path, const char * method, AeresObject::CArgs & args, AeresObject::Callback callback)
  {
    Json::Value data;
    auto req = CreateRequest(path, method, args, data);
    if (!req)
    {
      return false;
    }

    req->Post(data, callback);
    queue.Enqueue(req);
    return true;
  }


  bool AeresSession::Call(std::string & path, const char * method, AeresObject::CArgs & args, AeresObject::RawCallback callback)
  {
    Json::Value data;
    auto req = CreateRequest(path, method, args, data);
    if (!req)
    {
      return false;
    }

    req->Post(data, callback);
    queue.Enqueue(req);
    return true;
  }


  bool AeresSession::Call(std::string & path, const char * method, AeresObject::CArgs & args, const void * body, size_t bodyLen, AeresObject::Callback callback)
  {
    std::string pathCopy = path;
    std::stringstream ss;
    ss << base;
    ss << "/api/";
    std::size_t css = path.find("://");
    if (css == std::string::npos) { return false; }
    ss << pathCopy.replace(css, 2, "", 0);
    ss << '/';
    ss << method;
    ss << this->__EncodeArgs(args);
    std::string url = ss.str();

    HttpRequest * req = new HttpRequest(url.c_str(), config);
    if (req != NULL)
    {
      req->Post("application/octet-stream", body, bodyLen, callback);
      queue.Enqueue(req);
    }
    return true;
  }


  std::shared_ptr<AeresObject> AeresSession::CreateObject(const char * name, const char * path, const char * type)
  {
    return AeresTypes::Create(base.c_str(), name, path, type);
  }

  bool AeresSession::Login(std::string & username, std::string & password)
  {
    auto systemSecurity = std::static_pointer_cast<aeres::AeresSystemSecurityApi>(this->CreateObject("SystemSecurity", "system://Security", "SystemSecurity"));
    auto result = systemSecurity->LoginPassword(username, password);

    if(!result->Wait() || result->HasError())
    {
      return false;
    }

    bool stFound = config->Cookies().FindFirst("st", base.c_str(), "/") != config->Cookies().End();

    return stFound;
  }

  std::string AeresSession::LoginEndpoint(std::string & appId, std::string & endpointId)
  {
    auto systemSecurity = std::static_pointer_cast<aeres::AeresSystemSecurityApi>(this->CreateObject("SystemSecurity", "system://Security", "SystemSecurity"));
    auto result = systemSecurity->LoginEndpoint(appId, endpointId);

    if(!result->Wait() || result->HasError())
    {
      return "";
    }

    auto res = result->GetResult();

    return res.c_str();
  }



  std::string AeresSession::VerifyEndpointToken(std::string & et)
  {
    auto systemSecurity = std::static_pointer_cast<aeres::AeresSystemSecurityApi>(this->CreateObject("SystemSecurity", "system://Security", "SystemSecurity"));
    auto result = systemSecurity->VerifyEndpointToken(et);

    if(!result->Wait() || result->HasError())
    {
      return "";
    }

    auto res = result->GetResult();

    return res.c_str();
  }




  uint32_t AeresSession::GetTimeout() const
  {
    if (!this->config)
    {
      return 1;
    }

    return 1000 * (this->config->ConnectTimeout() + this->config->RequestTimeout()) * (1 + this->config->Relays().size());
  }
}
