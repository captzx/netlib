#include "SupervisorServer.h"

#include <boost/asio.hpp>
using namespace boost::asio;

using namespace x::supervisor;

/// SupervisorServer
SupervisorServer::SupervisorServer() {

}

void SupervisorServer::InitModule() {
	ProtobufDispatcher& dispatcher = SupervisorServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<PassiveHeartBeat>(std::bind(&SupervisorServer::OnPassiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
	
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