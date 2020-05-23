#include "SupervisorServer.h"

using x::supervisor::SupervisorServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	SupervisorServer::GetInstance().Start();

	return 0;
}