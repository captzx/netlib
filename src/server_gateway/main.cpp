#include "GatewayServer.h"

using x::gateway::GatewayServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	GatewayServer::GetInstance().Start();

	return 0;
}