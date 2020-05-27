#include "CenterServer.h"

using namespace x::center;

/// CenterServer
CenterServer::CenterServer(){

}

void CenterServer::InitModule() {

}


int32_t main(int32_t argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	CenterServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}