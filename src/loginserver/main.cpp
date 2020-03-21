#include "common.h"

#include "LoginServer.h"

int main() {
	global_logger_init();

	LoginServer& loginserver = LoginServer::GetInstance();

	loginserver.Init();

	loginserver.Start();

	return 0;
}