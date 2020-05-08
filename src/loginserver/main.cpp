#include <xtools/Common.h>
#include "LoginServer.h"

using x::login::LoginServer;

int main() {
	std::shared_ptr<LoginServer> pLoginServer = std::make_shared<LoginServer>("LoginServer");
	if (pLoginServer) pLoginServer->Start();

	return 0;
}