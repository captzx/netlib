#include "common.h"

#include "LoginServer.h"

int main() {
	LoginServer::GetInstance().Start();

	return 0;
}