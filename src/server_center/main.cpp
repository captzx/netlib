#include "CenterServer.h"

using x::center::CenterServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	CenterServer::GetInstance().Start();

	return 0;
}