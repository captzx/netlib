#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
GatewayServer::GatewayServer(){

}

void GatewayServer::InitModule() {
	ProtobufDispatcher& dispatcher = GatewayServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestPlayerLoginData>(std::bind(&GatewayServer::OnRequestPlayerLoginData, this, std::placeholders::_1, std::placeholders::_2));

}


void GatewayServer::OnRequestPlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<RequestPlayerLoginData>& pRecv) {
	auto it = _connections.find(ServerType::DATA);
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pRecv);

		log(warning, "GatewayServer") << "async send RequestPlayerLoginData, act_id = " << pRecv->act_id();
	}
}