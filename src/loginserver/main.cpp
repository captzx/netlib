#include "common.h"

#include "LoginServer.h"

using piece::login::LoginServer;

int main() {
	LoginServer::GetInstance().Start();

	return 0;
}