/*
  MIT License

  Copyright (c) 2018 aeres.io

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <fstream>
#include <aeres/ScopeGuard.h>
#include <aeres/Util.h>
#include <aeres/rule/InputRule.h>
#include <aeres/rule/ForwardRule.h>
#include <aeres/rule/RuleConfig.h>


namespace aeres
{
  namespace rule
  {
    void RuleConfig::AddRule(std::shared_ptr<RuleBase> rule, bool isDefault)
    {
      this->rules.push_back(rule);
      if (isDefault)
      {
        this->defaultRules.push_back(rule);
      }
    }


    bool RuleConfig::Load(const char * path)
    {
      ScopeGuard guard([this]() {
        this->rules.clear();
        for (const auto & rule : this->defaultRules)
        {
          this->rules.push_back(rule);
        }
      });

      this->rules.clear();

      std::ifstream stream(path);
      return_false_if(!stream);

      std::string line;
      while (std::getline(stream, line))
      {
        size_t pos = line.find_first_not_of(" \t\r\n");
        if (pos == std::string::npos || line[pos] == '#')
        {
          continue;
        }
        else
        {
          size_t end = line.find_first_of(" \t\r\n", pos);
          end = end == std::string::npos ? line.size() : end;
          std::string token = line.substr(pos, end - pos);
          if (token == "DEFAULT")
          {
            for (const auto & rule : this->defaultRules)
            {
              this->rules.push_back(rule);
            }
          }
          else
          {
            RuleType type = ToRuleType(token.c_str());
            std::shared_ptr<RuleBase> rule = RuleBase::Create(type);
            return_false_if(!rule || !rule->FromString(line.substr(pos)));
            this->rules.push_back(rule);
          }
        }
      }

      guard.Reset();
      return true;
    }


    bool RuleConfig::Save(const char * path) const
    {
      std::ofstream stream(path);
      return_false_if(!stream);

      for (const auto & rule : this->rules)
      {
        stream << rule->ToString() << std::endl;
      }

      return true;
    }
  }
}