#include <xtools/Common.h>
#include "LoginServer.h"

using x::login::LoginServer;

int main() {
	IOContext ctx;
	std::shared_ptr<LoginServer> pLoginServer = std::make_shared<LoginServer>(ctx, "LoginServer");
	if (pLoginServer) pLoginServer->Start();
	ctx.Run();
	return 0;
}