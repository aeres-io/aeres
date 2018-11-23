#include "AeresListener.h"

#include <aeres/Log.h>
#include <aeres/client/QuicClient.h>
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

    std::string et = session->LoginEndpoint(Options::applicationId,Options::endpointId);
    if (!et.empty())
    {
      aeres::client::QuicClient client{et, host, Options::port};

      if (!client.Start())
      {
        aeres::Log::Critical("Failed to start relay client.");
      }

      aeres::Log::Information("Relay client started.");

      client.WaitForExit();

      aeres::Log::Information("Prepare to restart relay client...");
    }
    else
    {
      aeres::Log::Information("Failed to login endpoint.");
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return true;
}

}
