#pragma once
#include <string>
#include <memory>
#include <string>

#include <xtools/Config.h>
#include <xtools/Singleton.h>
#include <xnet/NetServer.h>

using x::tool::Config;
using x::tool::Singleton;

namespace x {
namespace login {

/// class LoginServer
using x::net::NetServer;
using NetServerPtr = std::shared_ptr<NetServer>;
class LoginServer {
public:
	explicit LoginServer(std::string);

public:
	void Start();

private:
	NetServerPtr _pTcpServer;
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

