#include "LoginServer.h"

#include "LoginUserManager.h"
#include "ZoneServerManager.h"

using namespace x::login;
using namespace mysqlx;

/// LoginServer
LoginServer::LoginServer(){

}

void LoginServer::InitModule() {
	ProtobufDispatcher& dispatcher = LoginServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<PassiveHeartBeat>(std::bind(&LoginServer::OnPassiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));

	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback();

	ZoneServerManager::GetInstance().InitServerList();
	ZoneServerManager::GetInstance().RegisterMessageCallback();
}