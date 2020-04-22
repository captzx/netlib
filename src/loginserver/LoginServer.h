#pragma once
#include <string>
#include <memory>

#include <xtools/Singleton.h>

using x::tool::Singleton;

namespace x {

namespace net { class NetServer; }
using x::net::NetServer;

namespace login {

class LoginServer : public Singleton<LoginServer> {
public:
	LoginServer();

public:
	void Start();

private:
	bool _Init();

private:
	std::shared_ptr<NetServer> _pTcpServer;
};

} // namespace login

} // namespace x

