#pragma once

#include <xcommon/Server.h>

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

namespace x {
namespace login {

/// class LoginServer
class LoginServer : public Server, public Singleton<LoginServer> {
public:
	LoginServer();
	virtual ~LoginServer() {}

public:
	virtual ServerType GetServerType() override { return ServerType::LOGIN; }
	virtual void InitModule() override;
};

} // namespace login
} // namespace x

