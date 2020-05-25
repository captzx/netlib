#include "DataServer.h"

using x::data::DataServer;

int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	DataServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}