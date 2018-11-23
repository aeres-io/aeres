#pragma once

#include <string>

namespace aeres
{

class AeresTunnel
{
private:
  std::string endpointId;

public:
  AeresTunnel(std::string & endpointId);

  bool Process();
};

}
