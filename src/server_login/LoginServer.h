#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

using namespace x::tool;
using namespace x::net; 

namespace x {
namespace login {

/// class LoginServer
class LoginServer : public Server, public Singleton<LoginServer> {
public:
	LoginServer();
	virtual ~LoginServer() {}

public:
	virtual void InitModule() override;
	virtual ServerType GetServerType() override { return TYPE; }

public:
	const static ServerType TYPE = ServerType::LOGIN;

};

} // namespace login
} // namespace x

