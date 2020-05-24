#include "LoginServer.h"

#include "LoginUserManager.h"

using namespace x::login;

/// LoginServer
const ServerType LoginServer::TYPE;
LoginServer::LoginServer(){

}

void LoginServer::InitModule() {
	ProtobufDispatcher& dispatcher = LoginServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<PassiveHeartBeat>(std::bind(&LoginServer::OnPassiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));

	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback();
}
