#pragma once

#include "AeresSession.h"

namespace aeres
{

class AeresRule
{
private:
  std::shared_ptr<AeresSession> session;
  std::string application;
  std::string endpoint;

public:
  AeresRule(std::shared_ptr<AeresSession> session, std::string & application, std::string & endpoint);

  bool Process();
  bool List(std::vector<std::string> & rules);
  bool Show(std::string & rule);
  bool Add(std::string & rule, std::string & ruleSpec);
  bool Update(std::string & rule, std::string & ruleSpec);
  bool Remove(std::string & rule);
  bool RemoveAll();
  bool GetAll(std::string & rule, std::map<std::string, std::string> & properties);
  bool Get(std::string & rule, std::string & name, std::string & value);
  bool Set(std::string & rule, std::string & name, std::string & value);
  bool Del(std::string & rule, std::string & name);
};

}
