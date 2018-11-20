#include "AeresSession.h"

namespace aeres {

AeresSession::AeresSession(std::string & host, uint16_t port) :
  host(host), port(port)
{
}

bool AeresSession::Login(std::string & username, std::string & password)
{
  // TODO
  return false;
}

}
