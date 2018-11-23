#include "AeresRuleCli.h"
#include "Options.h"
#include "AeresEndpointApi.h"
#include "AeresRuleApi.h"

#include <iomanip>

namespace aeres {

AeresRuleCli::AeresRuleCli(std::shared_ptr<AeresSession> session, std::string & endpoint) :
  session(session),
  endpoint(endpoint)
{
}

bool AeresRuleCli::Process()
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
        return Remove(Options::ruleId);
      }
      case Action::GetAll:
      {
        return GetAll(Options::ruleId);
      }
      case Action::Get:
      {
        return Get(Options::ruleId, Options::arg1);
      }
      case Action::Set:
      {
        return Set(Options::ruleId, Options::arg1, Options::arg2);
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

bool AeresRuleCli::List()
{
  std::string path = "name://Endpoints/" + endpoint;
  auto apiObj = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  auto result = apiObj->GetRules();

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
    printf("%-20s%-50s\n", "RuleId", "Spec");
    for (size_t i = 0; i < jsonArray.size(); i++)
    {
      std::string specs = jsonArray[i]["Properties"]["Action"].asString() + ":" + jsonArray[i]["Properties"]["Domain"].asString() + ":" + jsonArray[i]["Properties"]["Port"].asString() + ":" + jsonArray[i]["Properties"]["Protocol"].asString();
      printf("%-20s%-50s\n", jsonArray[i]["Properties"]["RuleId"].asString().c_str(), specs.c_str());
    }
  }

  return true;
}

bool AeresRuleCli::Add(const char * spec)
{
  std::vector<std::string> tokens;
  std::istringstream tokenStream(spec);
  std::string token;
  while (std::getline(tokenStream, token, ':'))
  {
    if(token.length())
    {
      tokens.push_back(token);
    }
  }

  if(tokens.size() != 4)
  {
    std::cout<<"Invalid spec"<<std::endl;
    return false;
  }

  std::string path = "name://Endpoints/" + endpoint;
  auto apiObj = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));
  auto result = apiObj->NewRule(tokens[0], tokens[1], tokens[2], tokens[3]);

   if (!result->Wait() || result->HasError())
   {
     return false;
   }

   auto &jsonObj = result->GetResult();
   if (jsonObj.isNull())
   {
     return false;
   }

   auto rule = jsonObj["Path"].asString();
   std::string prefix = "name://Rules/";
   if(rule.find(prefix) != std::string::npos)
   {
     rule.erase(0,prefix.length());
   }

   std::cout << rule << std::endl;

  return true;
}

bool AeresRuleCli::RemoveAll()
{
  // TODO
  return false;
}

bool AeresRuleCli::Show(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresRuleCli::Remove(std::string & rule)
{
  std::string path = "name://Rules/" + rule;
  auto apiObj = std::static_pointer_cast<aeres::AeresRuleApi>(session->CreateObject("Rules", path.c_str(), "Rules"));

  auto result = apiObj->Delete();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  return result->GetResult();
}

bool AeresRuleCli::GetAll(std::string & rule)
{
  std::string path = "name://Rules/" + rule;
  auto apiObj = std::static_pointer_cast<AeresRuleApi>(session->CreateObject("Rules", path.c_str(), "Rules"));

  auto result = apiObj->GetProperties();

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

bool AeresRuleCli::Get(std::string & rule, std::string & name)
{
  std::string path = "name://Rules/" + rule;
  auto apiObj = std::static_pointer_cast<AeresRuleApi>(session->CreateObject("Rules", path.c_str(), "Rules"));

  bool res = false;

  if (name == "Description")
  {
    auto result = apiObj->GetDescription();
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresRuleCli::Set(std::string & rule, std::string & name, std::string & value)
{
  std::string path = "name://Rules/" + rule;
  auto apiObj = std::static_pointer_cast<AeresRuleApi>(session->CreateObject("Rules", path.c_str(), "Rules"));

  bool res = false;

  if (name == "Description")
  {
    auto result = apiObj->SetDescription(value);
    res = result->Wait() && !result->HasError();
    std::cout << result->GetResult() << std::endl;
  }

  return res;
}

bool AeresRuleCli::Del(std::string & endpoint, std::string & name)
{
  // TODO
  return false;
}
}
