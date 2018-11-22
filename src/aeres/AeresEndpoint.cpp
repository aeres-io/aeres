#include "AeresEndpoint.h"
#include "Options.h"
#include "AeresUserApi.h"
#include "AeresApplicationApi.h"
#include "AeresEndpointApi.h"

namespace aeres {

AeresEndpoint::AeresEndpoint(std::shared_ptr<AeresSession> session, std::string & application) :
  session(session),
  application(application)
{
}

bool AeresEndpoint::Process()
{
  if (session->Login(Options::username, Options::password))
  {
    switch (Options::action)
    {
      case Action::List:
      {
        std::vector<std::pair<std::string, std::string>> endpoints;
        if (List(endpoints))
        {
          if(endpoints.size())
          {
            printf("%-20s%-32s\n", "EndpointID", "Description");
          }
          for (auto &it : endpoints)
          {
            printf("%-20s%-32s\n", it.first.c_str(), it.second.c_str());
          }
          return true;
        }
        else
        {
          printf("Error: failed to retrieve application endpoints\n");
          return false;
        }
      }
      case Action::Add:
      {
        std::string endpointId;
        if (Add(endpointId))
        {
          printf("Created: %s\n", endpointId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to create endpoint\n");
          return false;
        }
      }
      case Action::Remove:
      {
        if (Remove(Options::endpointId))
        {
          printf("Removed: %s\n", Options::endpointId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to remove endpoint %s\n", Options::endpointId.c_str());
          return false;
        }
      }
      case Action::RemoveAll:
      {
        if (RemoveAll())
        {
          printf("Removed all endpoints\n");
          return true;
        }
        else
        {
          printf("Error: failed to remove all endpoints\n");
          return false;
        }
      }
      default:
      {
        printf("Unsupported action\n");
        return false;
      }
    }
  }
  else
  {
    printf("Invalid credentials\n");
    return false;
  }
}

bool AeresEndpoint::List(std::vector<std::pair<std::string, std::string>> & endpoints)
{
  std::string path = "name://Applications/" + Options::applicationId;
  auto appApi = std::static_pointer_cast<aeres::AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  auto result = appApi->GetEndpoints();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonArray = result->GetResult();
  if (jsonArray.isNull())
  {
    return false;
  }

  for(auto &json : jsonArray)
  {
    endpoints.emplace_back(std::make_pair(json["Properties"]["EndpointId"].asString(), json["Properties"]["Description"].asString()));
  }

  return true;
}

bool AeresEndpoint::Show(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresEndpoint::Add(std::string & endpoint)
{
  std::string path = "name://Applications/" + Options::applicationId;
  auto appApi = std::static_pointer_cast<aeres::AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  auto result = appApi->NewEndpoint();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonObj = result->GetResult();
  if (jsonObj.isNull())
  {
    return false;
  }

  endpoint = jsonObj["Path"].asString();

  std::string prefix = "name://Endpoints/";
  if(endpoint.find(prefix) != std::string::npos)
  {
    endpoint.erase(0,prefix.length());
  }

  return true;
}

bool AeresEndpoint::Remove(std::string & endpoint)
{
  std::string path = "name://Endpoints/" + endpoint;
  auto endApi = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoint", path.c_str(), "Endpoint"));

  auto result = endApi->Delete();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  return result->GetResult();
}

bool AeresEndpoint::RemoveAll()
{
  // TODO
  return false;
}

bool AeresEndpoint::GetAll(std::string & endpoint, std::map<std::string, std::string> & properties)
{
  // TODO
  return false;
}

bool AeresEndpoint::Get(std::string & endpoint, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresEndpoint::Set(std::string & endpoint, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresEndpoint::Del(std::string & endpoint, std::string & name)
{
  // TODO
  return false;
}

}
