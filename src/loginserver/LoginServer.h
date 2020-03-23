#pragma once
#include <string>
#include <memory>

#include "Singleton.h"

using piece::tool::Singleton;

namespace piece {

namespace net { class NetServer; }
using piece::net::NetServer;

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

} // namespace piece

