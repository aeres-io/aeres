#pragma once

#include "AeresSession.h"

namespace aeres
{

class AeresApplicationCli
{
private:
  std::shared_ptr<AeresSession> session;

public:
  AeresApplicationCli(std::shared_ptr<AeresSession> session);

  bool Process();
  bool List();
  bool Add(const char * description);
  bool RemoveAll();
  bool Show(std::string & application);
  bool Remove(std::string & application);
  bool GetAll(std::string & application);
  bool Get(std::string & application, std::string & name);
  bool Set(std::string & application, std::string & name, std::string & value);
  bool Del(std::string & application, std::string & name);
};

}
