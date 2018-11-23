#include "AeresRule.h"
#include "Options.h"
#include "AeresEndpointApi.h"

namespace aeres {

AeresRule::AeresRule(std::shared_ptr<AeresSession> session, std::string & application, std::string & endpoint) :
  session(session),
  application(application),
  endpoint(endpoint)
{
}

bool AeresRule::Process()
{
  if (session->Login(Options::username, Options::password))
  {
    switch (Options::action)
    {
      case Action::List:
      {
        std::vector<std::pair<std::string, std::string>> rules;
        if (List(rules))
        {
          if(rules.size())
          {
            printf("%-40s%-50s\n", "RuleID", "Spec");
          }
          for (auto &it : rules)
          {
            printf("%-40s%-50s\n", it.first.c_str(), it.second.c_str());
          }
          return true;
        }
        else
        {
          printf("Error: failed to retrieve endpoint rules\n");
          return false;
        }
      }
      case Action::Add:
      {
        std::string ruleId, specs;
        if (Add(ruleId, specs))
        {
          printf("Created: %s\n", ruleId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to create endpoint rule\n");
          return false;
        }
      }
      case Action::Remove:
      {
        if (Remove(Options::ruleId))
        {
          printf("Removed: %s\n", Options::ruleId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to remove rule %s\n", Options::ruleId.c_str());
          return false;
        }
      }
      case Action::RemoveAll:
      {
        if (RemoveAll())
        {
          printf("Removed all endpoint rules\n");
          return true;
        }
        else
        {
          printf("Error: failed to remove all endpoint rules\n");
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

bool AeresRule::List(std::vector<std::pair<std::string, std::string>> & rules)
{
  std::string path = "name://Endpoints/" + Options::endpointId;
  auto endApi = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  auto result = endApi->GetRules();

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
    if(json["Properties"].isObject())
    {
      rules.emplace_back(std::make_pair(json["Properties"]["RuleId"].asString(), json["Properties"]["Spec"].asString()));
    }
  }

  return true;
}

bool AeresRule::Show(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresRule::Add(std::string & rule, std::string & ruleSpec)
{
  std::string path = "name://Endpoints" + Options::endpointId;
  auto endApi = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoints", path.c_str(), "Endpoints"));

  auto result = endApi->NewRule(ruleSpec.c_str());

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  auto &jsonObj = result->GetResult();
  if (jsonObj.isNull())
  {
    return false;
  }

  rule = jsonObj["Path"].asString();

  std::string prefix = "name://Rules/";
  if(rule.find(prefix) != std::string::npos)
  {
    rule.erase(0,prefix.length());
  }

  return true;
}

bool AeresRule::Remove(std::string & endpoint)
{
  /*std::string path = "name://Endpoints/" + endpoint;
  auto endApi = std::static_pointer_cast<aeres::AeresEndpointApi>(session->CreateObject("Endpoint", path.c_str(), "Endpoint"));

  auto result = endApi->Delete();

  if (!result->Wait() || result->HasError())
  {
    return false;
  }

  return result->GetResult();
}*/
  return false;
}


bool AeresRule::Update(std::string & rule, std::string & ruleSpec)
{
  // TODO
  return false;
}

bool AeresRule::RemoveAll()
{
  // TODO
  return false;
}

bool AeresRule::GetAll(std::string & rule, std::map<std::string, std::string> & properties)
{
  // TODO
  return false;
}

bool AeresRule::Get(std::string & rule, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresRule::Set(std::string & rule, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresRule::Del(std::string & rule, std::string & name)
{
  // TODO
  return false;
}

}
