#pragma once

#include <xcommon/Server.h>
#include <xprotos/Server.pb.h>

namespace x {
namespace supervisor {

/// class SupervisorServer
class SupervisorServer : public Server, public Singleton<SupervisorServer> {
public:
	SupervisorServer();
	virtual ~SupervisorServer(){ }

public:
	virtual ServerType GetServerType() override { return ServerType::SUPERVISOR; }
	virtual void InitModule() override;

public:
	void AsyncHeartBeatInLoop(std::shared_ptr<boost::asio::deadline_timer> pTimer);
};

} // namespace supervisor
} // namespace x

