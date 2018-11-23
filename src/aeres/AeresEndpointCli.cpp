#include "AeresEndpointCli.h"

#include "Options.h"
#include "AeresUserApi.h"
#include "AeresEndpointsApi.h"
#include "AeresEndpointApi.h"

#include <iomanip>

namespace aeres {

AeresEndpointCli::AeresEndpointCli(std::shared_ptr<AeresSession> session) :
  session(session)
{
}

bool AeresEndpointCli::Process()
{
  if (session->Login(Options::username, Options::password))
  {
    switch (Options::action)
    {
      case Action::List:
      {
        return List();
      }
      case Action::Add:
      {
        return Add(Options::args.size() ? Options::arg1.c_str() : nullptr);
      }
      case Action::RemoveAll:
      {
        return RemoveAll();
      }
      case Action::Remove:
      {
        return Remove(Options::endpointId);
      }
      case Action::GetAll:
      {
        return GetAll(Options::endpointId);
      }
      case Action::Get:
      {
        return Get(Options::endpointId, Options::arg1);
      }
      case Action::Set:
      {
        return Set(Options::endpointId, Options::arg1, Options::arg2);
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

bool AeresEndpointCli::List()
{
  auto endpointsApi = std::static_pointer_cast<aeres::AeresEndpointsApi>(session->CreateObject("Endpoints", "name://Endpoints", "Endpoints"));
  auto result = endpointsApi->GetEndpoints();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonArray = result->GetResult();
  if (jsonArray.isNull())
  {
    return false;
  }

  if(jsonArray.size())
  {
    printf("%-20s%-32s\n", "EndpointID", "Description");
    for (size_t i = 0; i < jsonArray.size(); i++)
    {
      printf("%-20s%-32s\n", jsonArray[i]["Properties"]["EndpointId"].asString().c_str(), jsonArray[i]["Properties"]["Description"].asString().c_str());
    }
  }

  return true;
}

bool AeresEndpointCli::Add(const char * description)
{
  auto endpointsApi = std::static_pointer_cast<aeres::AeresEndpointsApi>(session->CreateObject("Endpoints", "name://Endpoints", "Endpoints"));
  auto result = endpointsApi->NewEndpoint(description);

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonObj = result->GetResult();
  if (jsonObj.isNull())
  {
    return false;
  }

  auto endpoint = jsonObj["Path"].asString();

  std::string prefix = "name://Endpoints/";
  if(endpoint.find(prefix) != std::string::npos)
  {
    endpoint.erase(0,prefix.length());
  }

  std::cout << endpoint << std::endl;

  return true;
}

bool AeresEndpointCli::RemoveAll()
{
  // TODO
  return false;
}

bool AeresEndpointCli::Show(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresEndpointCli::Remove(std::string & endpoint)
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

bool AeresEndpointCli::GetAll(std::string & endpoint)
{
  std::string path = "name://Endpoints/" + endpoint;
  auto epApi = std::static_pointer_cast<AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  auto result = epApi->GetProperties();

  if (!result->Wait() || result->HasError())
  {
    std::cout << result->GetResult();

    return false;
  }

  auto json = result->GetResult();
  auto names = json.getMemberNames();
  for (auto itr = names.begin(); itr != names.end(); itr++)
  {
    std::cout << std::left << std::setw(25) << (*itr) << json[(*itr)] << std::endl;
  }

  return true;
}

bool AeresEndpointCli::Get(std::string & endpoint, std::string & name)
{
  std::string path = "name://Endpoints/" + endpoint;
  auto epApi = std::static_pointer_cast<AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  bool res = false;

  if (name == "Description")
  {
    auto result = epApi->GetDescription();
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresEndpointCli::Set(std::string & endpoint, std::string & name, std::string & value)
{
  std::string path = "name://Endpoints/" + endpoint;
  auto epApi = std::static_pointer_cast<AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  bool res = false;

  if (name == "Description")
  {
    auto result = epApi->SetDescription(value);
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresEndpointCli::Del(std::string & endpoint, std::string & name)
{
  // TODO
  return false;
}

}
