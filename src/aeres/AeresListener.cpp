#include "AeresListener.h"

#include <aeres/Log.h>
#include <aeres/client/QuicClient.h>
#include "Options.h"

namespace aeres
{

AeresListener::AeresListener(std::string & applicationId, std::string & endpointId) :
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

    aeres::client::QuicClient client{Options::applicationId, host, Options::port};

    if (!client.Start())
    {
      aeres::Log::Critical("Failed to start relay client.");
    }

    aeres::Log::Information("Relay client started.");

    client.WaitForExit();

    aeres::Log::Information("Prepare to restart relay client...");

    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return true;
}

}
