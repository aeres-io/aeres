#include "AeresRule.h"

namespace aeres {

AeresRule::AeresRule(std::shared_ptr<AeresSession> session, std::string & application, std::string & endpoint) :
  session(session),
  application(application),
  endpoint(endpoint)
{
}

bool AeresRule::Process()
{
  // TODO
  return false;
}

bool AeresRule::List(std::vector<std::string> & rules)
{
  // TODO
  return false;
}

bool AeresRule::Show(std::string & rule)
{
  // TODO
  return false;
}

bool AeresRule::Add(std::string & rule, std::string & ruleSpec)
{
  // TODO
  return false;
}

bool AeresRule::Update(std::string & rule, std::string & ruleSpec)
{
  // TODO
  return false;
}

bool AeresRule::Remove(std::string & rule)
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
