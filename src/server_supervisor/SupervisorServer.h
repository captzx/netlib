#pragma once
#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>

using namespace x::tool;
using namespace x::net; 

namespace x {
namespace supervisor {

/// class SupervisorServer
class SupervisorServer : public Singleton<SupervisorServer> {
public:
	explicit SupervisorServer();

public:
	void Start(); 
	
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnConnection(const TcpConnectionPtr&);

private:
	ServerCfg* _pSvrCfg;

	TcpServicePtr _pTcpService;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
};

} // namespace login
} // namespace x

