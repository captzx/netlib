#include "LoginServer.h"

using x::login::LoginServer;

int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	LoginServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}