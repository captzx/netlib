#pragma once
#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

using namespace x::tool;
using namespace x::net; 

namespace x {
namespace login {

/// class LoginServer
class LoginServer : public Singleton<LoginServer> {
public:
	explicit LoginServer();

public:
	void Start(); 
	bool SendHeartBeat(const TcpConnectionPtr&);
	
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnConnection(const TcpConnectionPtr&);
	void OnHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<HeartBeat>&);

public:
	DBConnectionPtr GetLoginDBConnection() { return _pDBLoginConnection; }

private:
	ServerCfg* _pSvrCfg;

	TcpServicePtr _pTcpService;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;

	TcpConnectionPtr _pGateWayConnection;
	DBConnectionPtr _pDBLoginConnection;

};

} // namespace login
} // namespace x

