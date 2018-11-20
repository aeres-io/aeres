#pragma once

#include <string>

namespace aeres
{

class AeresListener
{
private:
  std::string applicationId;
  std::string endpointId;

public:
  AeresListener(std::string & applicationId, std::string & endpointId);

  bool Process();
};

}
