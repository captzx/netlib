#include "LoginServer.h"

#include "LoginUserManager.h"
#include "ZoneServerManager.h"

using namespace x::login;

/// LoginServer
LoginServer::LoginServer(){

}

void LoginServer::InitModule() {
	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback();

	ZoneServerManager::GetInstance().InitServerList();
	ZoneServerManager::GetInstance().RegisterMessageCallback();
}

int32_t main(int32_t argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	LoginServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}