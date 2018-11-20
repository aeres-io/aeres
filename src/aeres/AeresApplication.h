#pragma once

#include "AeresSession.h"

namespace aeres
{

class AeresApplication
{
private:
  std::shared_ptr<AeresSession> session;

public:
  AeresApplication(std::shared_ptr<AeresSession> session);

  bool Process();
  bool List(std::vector<std::string> & applications);
  bool Show(std::string & application);
  bool Add(std::string & application);
  bool Remove(std::string & application);
  bool RemoveAll();
  bool GetAll(std::string & application, std::map<std::string, std::string> & properties);
  bool Get(std::string & application, std::string & name, std::string & value);
  bool Set(std::string & application, std::string & name, std::string & value);
  bool Del(std::string & application, std::string & name);
};

}
