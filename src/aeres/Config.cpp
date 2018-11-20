#include "Config.h"

#include <fstream>

namespace aeres
{

Config::Config(std::string & path) :
  path(path)
{
}

bool Config::Load()
{
  std::ifstream jsonStream(path);
  if (jsonStream)
  {
    Json::Reader reader;
    return reader.parse(jsonStream, root);
  }
  return false;
}

bool Config::Save()
{
  std::ofstream jsonStream(path);
  if (jsonStream)
  {
    jsonStream << root;
    return true;
  }
  return false;
}

Json::Value & Config::Root()
{
  return root;
}

}
