#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
const ServerType GatewayServer::TYPE;

GatewayServer::GatewayServer(){

}

void GatewayServer::InitModule() {
	ProtobufDispatcher& dispatcher = GatewayServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<PassiveHeartBeat>(std::bind(&GatewayServer::OnPassiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
}