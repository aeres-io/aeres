#include "AeresApplicationCli.h"
#include "Options.h"
#include "AeresApplicationApi.h"
#include "AeresUserApi.h"

#include <iomanip>

namespace aeres {

AeresApplicationCli::AeresApplicationCli(std::shared_ptr<AeresSession> session) :
  session(session)
{
}

bool AeresApplicationCli::Process()
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
        return Remove(Options::applicationId);
      }
      case Action::GetAll:
      {
        return GetAll(Options::applicationId);
      }
      case Action::Get:
      {
        return Get(Options::applicationId, Options::arg1);
      }
      case Action::Set:
      {
        return Set(Options::applicationId, Options::arg1, Options::arg2);
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

bool AeresApplicationCli::List()
{
  std::string path = "name://Users/" + Options::username;
  auto userApi = std::static_pointer_cast<aeres::AeresUserApi>(session->CreateObject("User", path.c_str(), "User"));

  auto result = userApi->GetApplications();

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
    printf("%-40s%-32s\n", "AppID", "Description");
    for (size_t i = 0; i < jsonArray.size(); i++)
    {
      printf("%-40s%-32s\n", jsonArray[i]["Properties"]["AppId"].asString().c_str(), jsonArray[i]["Properties"]["Description"].asString().c_str());
    }
  }

  return true;
}

bool AeresApplicationCli::Add(const char * description)
{
  auto appApi = std::static_pointer_cast<AeresApplicationApi>(session->CreateObject("Applications", "name://Applications", "Applications"));

  auto result = appApi->NewApplication(description);

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonObj = result->GetResult();
  if (jsonObj.isNull())
  {
    return false;
  }

  std::cout << jsonObj;

  auto application = jsonObj["Path"].asString();

  auto applicationId = application.substr(20);

  printf("%s\n", applicationId.c_str());

  return true;
}

bool AeresApplicationCli::RemoveAll()
{
  // TODO
  return false;
}

bool AeresApplicationCli::Show(std::string & application)
{
  return true;
}

bool AeresApplicationCli::Remove(std::string & application)
{
  std::string path = "name://Applications/" + application;
  auto appApi = std::static_pointer_cast<AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  auto result = appApi->Delete();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  return result->GetResult();
}

bool AeresApplicationCli::GetAll(std::string & application)
{
  std::string path = "name://Applications/" + application;
  auto appApi = std::static_pointer_cast<AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  auto result = appApi->GetProperties();

  if (!result->Wait() || result->HasError())
  {
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

bool AeresApplicationCli::Get(std::string & application, std::string & name)
{
  std::string path = "name://Applications/" + application;
  auto appApi = std::static_pointer_cast<AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  bool res = false;

  if (name == "Description")
  {
    auto result = appApi->GetDescription();
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresApplicationCli::Set(std::string & application, std::string & name, std::string & value)
{
  std::string path = "name://Applications/" + application;
  auto appApi = std::static_pointer_cast<AeresApplicationApi>(session->CreateObject("Applications", path.c_str(), "Applications"));

  bool res = false;

  if (name == "Description")
  {
    auto result = appApi->SetDescription(value);
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresApplicationCli::Del(std::string & application, std::string & name)
{
  // TODO
  return false;
}

}
