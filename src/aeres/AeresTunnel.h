#pragma once

#include <string>

namespace aeres
{

class AeresTunnel
{
private:
  std::string endpointId;
  std::string key;

public:
  AeresTunnel(std::string & endpointId, std::string & key);

  bool Process();
};

}
