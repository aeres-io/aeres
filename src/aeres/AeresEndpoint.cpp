#include "AeresEndpoint.h"

namespace aeres {

AeresEndpoint::AeresEndpoint(std::shared_ptr<AeresSession> session, std::string & application) :
  session(session),
  application(application)
{
}

bool AeresEndpoint::Process()
{
  // TODO
  return false;
}

bool AeresEndpoint::List(std::vector<std::string> & endpoints)
{
  // TODO
  return false;
}

bool AeresEndpoint::Show(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresEndpoint::Add(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresEndpoint::Remove(std::string & endpoint)
{
  // TODO
  return false;
}

bool AeresEndpoint::RemoveAll()
{
  // TODO
  return false;
}

bool AeresEndpoint::GetAll(std::string & endpoint, std::map<std::string, std::string> & properties)
{
  // TODO
  return false;
}

bool AeresEndpoint::Get(std::string & endpoint, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresEndpoint::Set(std::string & endpoint, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresEndpoint::Del(std::string & endpoint, std::string & name)
{
  // TODO
  return false;
}

}
