#include <xtools/Common.h>
#include "LoginServer.h"

using x::login::LoginServer;

int main() {
	LoginServer logServer("LogServer");
	logServer.Start();
	while (1);
	return 0;
}