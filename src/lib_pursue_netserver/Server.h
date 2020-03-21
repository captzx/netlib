#pragma once

#include "NetMoudle.h"

class Server {
public:
	void Init();
	void Listen(unsigned int port);

private:
	NetMoudle _netMoudle;
};