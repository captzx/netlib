#include "LoginServer.h"

#include "common.h"

#include "LoginConfig.h"
#include <xnet/NetServer.h>


using namespace x::login;
using namespace x::tool;

LoginServer::LoginServer(){
	_pTcpServer = std::make_shared<NetServer>();
}

void LoginServer::Start() {
	_Init();

	_pTcpServer->Listen(LoginConfig::GetInstance().GetListenPort());
}

bool LoginServer::_Init() {
	LoginConfig::GetInstance().LoadFile("config.xml");

	global_logger_init(LoginConfig::GetInstance().GetLogFile());

	_pTcpServer->Init();

	return true;
}
