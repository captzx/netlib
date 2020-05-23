#include "LoginServer.h"

using x::login::LoginServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	LoginServer::GetInstance().Start();

	return 0;
}