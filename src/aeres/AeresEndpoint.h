#pragma once

#include "AeresSession.h"

namespace aeres
{

class AeresEndpoint
{
private:
  std::shared_ptr<AeresSession> session;
  std::string application;

public:
  AeresEndpoint(std::shared_ptr<AeresSession> session, std::string & application);

  bool Process();
  bool List(std::vector<std::string> & endpoints);
  bool Show(std::string & endpoint);
  bool Add(std::string & endpoint);
  bool Remove(std::string & endpoint);
  bool RemoveAll();
  bool GetAll(std::string & endpoint, std::map<std::string, std::string> & properties);
  bool Get(std::string & endpoint, std::string & name, std::string & value);
  bool Set(std::string & endpoint, std::string & name, std::string & value);
  bool Del(std::string & endpoint, std::string & name);
};

}
