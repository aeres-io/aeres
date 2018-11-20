#pragma once

#include <map>
#include <string>
#include <vector>
#include <memory>

namespace aeres
{

class AeresSession
{
private:
  std::string host;
  uint16_t port;

public:
  AeresSession(std::string & host, uint16_t port);
  bool Login(std::string & username, std::string & password);
};

}
