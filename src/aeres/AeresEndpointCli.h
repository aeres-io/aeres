#pragma once

#include "AeresSession.h"

namespace aeres
{

class AeresEndpointCli
{
private:
  std::shared_ptr<AeresSession> session;

public:
  AeresEndpointCli(std::shared_ptr<AeresSession> session);

  bool Process();
  bool List();
  bool Add(const char * description);
  bool RemoveAll();
  bool Show(std::string & endpoint);
  bool Remove(std::string & endpoint);
  bool GetAll(std::string & endpoint);
  bool Get(std::string & endpoint, std::string & name);
  bool Set(std::string & endpoint, std::string & name, std::string & value);
  bool Del(std::string & endpoint, std::string & name);
};

}
