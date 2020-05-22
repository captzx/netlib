#include <xtools/Common.h>
#include "GatewayServer.h"

using x::gateway::GatewayServer;

int main() {
	GatewayServer gatewayServer("GatewayServer");
	gatewayServer.Start();
	while (1);
	return 0;
}