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
namespace login {

/// class LoginServer
class LoginServer {
public:
	explicit LoginServer(IOContext&, std::string);

public:
	void Start(); 
	bool SendHeartBeat(const TcpConnectionPtr&);
	
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnConnection(const TcpConnectionPtr&);
	void OnHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<HeartBeat>&);

private:
	TcpServerPtr _pTcpServer;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
	
public:
	static DBConnectionPtr GetLoginDBConnection() { return _pDBLoginConnection; }
public:
	static DBConnectionPtr _pDBLoginConnection;
};

/// class LoginConfig
class LoginConfig : public Config, public Singleton<LoginConfig> {
	friend class LoginServer;
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

