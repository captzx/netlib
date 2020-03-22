#pragma once
#include <string>

#include "Singleton.h"
#include "NetServer.h"

using piece::net::NetServer;
using piece::tool::Singleton;

struct lsCfg {
	std::string file;
	unsigned int port = 0;
};

class LoginServer : public Singleton<LoginServer> {
public:
	void Start();

	bool ReloadConfig();

private:
	bool _LoadConfig(std::string file);

	bool _Init();

private:
	NetServer _tcpServer;
	lsCfg _lsCfg;
};
