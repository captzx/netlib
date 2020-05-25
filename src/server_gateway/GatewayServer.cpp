#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
GatewayServer::GatewayServer(){

}

void GatewayServer::InitModule() {
	ProtobufDispatcher& dispatcher = GatewayServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestPlayerLoginData>(std::bind(&GatewayServer::OnRequestPlayerLoginData, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ResponsePlayerLoginData>(std::bind(&GatewayServer::OnResponsePlayerLoginData, this, std::placeholders::_1, std::placeholders::_2));
	
}


void GatewayServer::OnRequestPlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<RequestPlayerLoginData>& pRecv) {
	auto it = _connections.find({ (unsigned int)ServerType::DATA, 1 }); // ??
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pRecv);

		log(warning, "GatewayServer") << "async send RequestPlayerLoginData, act_id = " << pRecv->act_id();
	}
}


void GatewayServer::OnResponsePlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<ResponsePlayerLoginData>& pRecv) {
	auto it = _connections.find({ (unsigned int)ServerType::LOGIN, pRecv->svr_id() });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pRecv);

		log(warning, "GatewayServer") << "async send RequestPlayerLoginData, act_id = " << pRecv->act_id();
	}
}