#include "common.h"

#include "Server.h"

void Server::Init() {
	log(normal) << "Server Init!";

	_netMoudle.Init();
}
void Server::Listen(unsigned int port) {
	log(normal) << "Server Init!";

	_netMoudle.Listen(port);
}