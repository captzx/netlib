#include "common.h"

#include "LoginServer.h"

using x::login::LoginServer;

int main() {
	LoginServer::GetInstance().Start();

	return 0;
}