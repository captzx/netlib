#include "SupervisorServer.h"

#include <boost/asio.hpp>
using namespace boost::asio;

using namespace x::supervisor;

/// SupervisorServer
SupervisorServer::SupervisorServer() {

}

void SupervisorServer::InitModule() {
	std::shared_ptr<deadline_timer> pTimer = std::make_shared<deadline_timer>(_pTcpService->GetIOContext());
	pTimer->expires_from_now(boost::posix_time::seconds(_heartbeatPeriod));
	AsyncHeartBeatInLoop(pTimer);
}

void SupervisorServer::AsyncHeartBeatInLoop(std::shared_ptr<deadline_timer> pTimer) {
	pTimer->async_wait([this, pTimer](const error_code& ec) {
		CheckHeartBeat();

		pTimer->expires_from_now(boost::posix_time::seconds(_heartbeatPeriod));
		AsyncHeartBeatInLoop(pTimer);
		}
	);
}

int32_t main(int32_t argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	SupervisorServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}