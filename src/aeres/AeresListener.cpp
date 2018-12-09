#include "AeresListener.h"

#include <aeres/Log.h>
#include <aeres/client/QuicClient.h>
#include <aeres/rule/RuleConfig.h>
#include <AeresSystemSecurityApi.h>
#include "Options.h"

namespace aeres
{

AeresListener::AeresListener(std::shared_ptr<AeresSession> session, std::string & applicationId, std::string & endpointId) :
  session(session),
  applicationId(applicationId),
  endpointId(endpointId)
{
}

bool AeresListener::Process()
{
  std::string host;
  size_t pos = Options::host.find("://");
  if (pos != std::string::npos)
  {
    host = Options::host.substr(pos + 3);
  }
  else
  {
    host = Options::host;
  }

  while (true)
  {
    aeres::Log::Information("Starting relay client...");

    auto api = std::static_pointer_cast<aeres::AeresSystemSecurityApi>(session->CreateObject("SystemSecurity", "system://Security", "SystemSecurity"));
    auto result = api->LoginEndpoint(Options::applicationId, Options::endpointId);

    if(!result->Wait() || result->HasError())
    {
      aeres::Log::Critical("Failed to login endpoint.");
    }
    else
    {
      const auto & obj = result->GetResult();
      if (obj.et.empty())
      {
        aeres::Log::Critical("Failed to login endpoint: et is empty.");
      }
      else
      {
        rule::RuleConfig ruleConfig;
        for (const auto & line : obj.tunnelRules)
        {
          auto rule = rule::RuleBase::Create(line);
          if (rule)
          {
            ruleConfig.AddRule(std::move(rule), true);
          }
        }

        if (!ruleConfig.Load(Options::rulesFile.c_str()))
        {
          aeres::Log::Verbose("No tunnel rules found locally. Use default rules only.");
        }

        aeres::client::QuicClient client{obj.et, host, Options::port, &ruleConfig};

        if (!client.Start())
        {
          aeres::Log::Critical("Failed to start relay client.");
        }

        aeres::Log::Information("Relay client started.");

        client.WaitForExit();

        aeres::Log::Information("Prepare to restart relay client...");
      }
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return true;
}

}
