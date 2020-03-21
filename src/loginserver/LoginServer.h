#pragma once

#include <pursue_tools/Singleton.h>
#include <pursue_netserver/Server.h>

struct lsCfg {
	unsigned int port = 0;
};

class LoginServer : public Singleton<LoginServer> {
public:
	void Init();
	void LoadConfig(std::string file);
	void Start();

private:
	Server _tcpServer;
	lsCfg _lsCfg;
};
