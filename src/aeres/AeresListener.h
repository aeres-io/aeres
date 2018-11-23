#pragma once

#include "AeresSession.h"
#include <string>

namespace aeres
{

class AeresListener
{
private:
  std::shared_ptr<AeresSession> session;
  std::string applicationId;
  std::string endpointId;

public:
  AeresListener(std::shared_ptr<AeresSession> session, std::string & applicationId, std::string & endpointId);

  bool Process();
};

}
