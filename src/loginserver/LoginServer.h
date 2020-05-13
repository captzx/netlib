#pragma once
#include <xtools/Common.h>
#include <xtools/Config.h>
#include <xtools/Singleton.h>
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
	void SendHeartBeat(const TcpConnectionPtr&);
public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void OnHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<HeartBeat>&);
	void OnSearchRequest(const TcpConnectionPtr&, const std::shared_ptr<SearchRequest>&);
	void OnSearchResponse(const TcpConnectionPtr&, const std::shared_ptr<SearchResponse>&);
	void OnConnection(const TcpConnectionPtr&);
private:
	TcpServerPtr _pTcpServer;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
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

