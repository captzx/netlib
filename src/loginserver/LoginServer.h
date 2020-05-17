#pragma once
#include <xtools/Common.h>
#include <xtools/Config.h>
#include <xtools/Singleton.h>
#include <xtools/Crypto.h>

#include <xnet/Using.h>
#include <xnet/TcpService.h>
#include <xnet/ProtobufProcess.h>

#include <xprotos/login.pb.h>

using x::tool::Config;
using x::tool::Singleton;

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
	bool SendRegisterResult(const TcpConnectionPtr& pConnection, int result);
	
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnConnection(const TcpConnectionPtr&);
	void OnHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<HeartBeat>&);
	void OnRequestRegister(const TcpConnectionPtr&, const std::shared_ptr<RequestRegister>&);
	void OnRequestRsaPublicKey(const TcpConnectionPtr&, const std::shared_ptr<RequestRsaPublicKey>&);

public:
	bool VerifyPassword(const std::string& account, const std::string& password);
	bool VerifyAccount(const std::string& account);
	bool SaveAccount(const std::string& account, const std::string& password);

private:
	TcpServerPtr _pTcpServer;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
	std::string _private_key;
	std::string _public_key;
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

