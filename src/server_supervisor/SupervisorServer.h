#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>

namespace x {
namespace supervisor {

/// class SupervisorServer
class SupervisorServer : public Server, public Singleton<SupervisorServer> {
public:
	SupervisorServer();
	virtual ~SupervisorServer(){ }

public:
	virtual void InitModule() override;
	virtual ServerType GetServerType() override { return TYPE; }

public:
	void AsyncHeartBeatInLoop(std::shared_ptr<boost::asio::deadline_timer> pTimer);
public: 
	const static ServerType TYPE = ServerType::SUPERVISOR;
};

} // namespace supervisor
} // namespace x

