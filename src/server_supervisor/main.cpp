#include "SupervisorServer.h"

using x::supervisor::SupervisorServer;

int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	SupervisorServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}