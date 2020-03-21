#include "common.h"

#include "LoginServer.h"


void LoginServer::Init() {
	log(normal) << "LoginServer Init!";

	_tcpServer.Init();
}


void LoginServer::LoadConfig(std::string file) {
	if (file.empty()) _lsCfg.port = 12345;

}

void LoginServer::Start() {
	log(normal) << "LoginServer Start!";
	LoadConfig("");
	_tcpServer.Listen(_lsCfg.port);
}

