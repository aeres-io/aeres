#include "AeresApplication.h"
#include "Options.h"

namespace aeres {

AeresApplication::AeresApplication(std::shared_ptr<AeresSession> session) :
  session(session)
{
}

bool AeresApplication::Process()
{
  if (session->Login(Options::username, Options::password))
  {
    switch (Options::action)
    {
      case Action::List:
      {
        std::vector<std::string> applications;
        if (List(applications))
        {
          for (auto itr = applications.begin(); itr != applications.end(); itr++)
          {
            printf("%s\n", (*itr).c_str());
          }
          return true;
        }
        else
        {
          printf("Error: failed to retrieve application list\n");
          return false;
        }
      }
      case Action::Add:
      {
        std::string applicationId;
        if (Add(applicationId))
        {
          printf("Created: %s\n", applicationId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to create application\n");
          return false;
        }
      }
      case Action::Remove:
      {
        if (Remove(Options::applicationId))
        {
          printf("Removed: %s\n", Options::applicationId.c_str());
          return true;
        }
        else
        {
          printf("Error: failed to remove application %s\n", Options::applicationId.c_str());
          return false;
        }
      }
      case Action::RemoveAll:
      {
        if (RemoveAll())
        {
          printf("Removed all applications\n");
          return true;
        }
        else
        {
          printf("Error: failed to remove all applications\n");
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

bool AeresApplication::List(std::vector<std::string> & applications)
{
  // TODO
  return false;
}

bool AeresApplication::Show(std::string & application)
{
  // TODO
  return false;
}

bool AeresApplication::Add(std::string & application)
{
  // TODO
  return false;
}

bool AeresApplication::Remove(std::string & application)
{
  // TODO
  return false;
}

bool AeresApplication::RemoveAll()
{
  // TODO
  return false;
}

bool AeresApplication::GetAll(std::string & application, std::map<std::string, std::string> & properties)
{
  // TODO
  return false;
}

bool AeresApplication::Get(std::string & application, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresApplication::Set(std::string & application, std::string & name, std::string & value)
{
  // TODO
  return false;
}

bool AeresApplication::Del(std::string & application, std::string & name)
{
  // TODO
  return false;
}

}
