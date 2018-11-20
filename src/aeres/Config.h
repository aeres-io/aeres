#pragma once

#include <string>
#include <json/json.h>

namespace aeres
{
  class Config
  {
  private:
    std::string path;
    Json::Value root;

  public:
    Config(std::string & path);

    bool Load();
    bool Save();

    Json::Value & Root();
  };
}
