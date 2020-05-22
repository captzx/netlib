#pragma once
#include <xtools/Common.h>
#include <xtools/Config.h>
#include <xtools/Singleton.h>
#include <xtools/Crypto.h>
#include <xtools/DBConnection.h>

#include <xnet/Using.h>
#include <xnet/TcpService.h>
#include <xnet/ProtobufProcess.h>

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

using namespace x::tool;
using namespace x::net; 

namespace x {
namespace supervisor {

/// class SupervisorServer
class SupervisorServer {
public:
	explicit SupervisorServer(std::string);

public:
	void Start(); 
	
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnConnection(const TcpConnectionPtr&);

private:
	TcpServicePtr _pTcpService;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
};

/// class LoginConfig
class GatewayConfig : public Config, public Singleton<GatewayConfig> {
	friend class SupervisorServer;
public:
	virtual bool Parse() override;

public:
	unsigned int GetListenPort() { return _netPort; }
	const std::string& GetLogFile() { return _logFile; }

private:
	std::string _logFile;
	unsigned int _netPort;
};

} // namespace login
} // namespace x

