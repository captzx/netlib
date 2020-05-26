#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
GatewayServer::GatewayServer(){

}

void GatewayServer::InitModule() {
	ProtobufDispatcher& dispatcher = GatewayServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestZoneRoleData>(std::bind(&GatewayServer::OnRequestZoneRoleData, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ResponseZoneRoleData>(std::bind(&GatewayServer::OnResponseZoneRoleData, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqCreateRole>(std::bind(&GatewayServer::OnReqCreateRole, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RspCreateRole>(std::bind(&GatewayServer::OnRspCreateRole, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqEnterGame>(std::bind(&GatewayServer::OnReqEnterGame, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<LoginIntoScene>(std::bind(&GatewayServer::OnLoginIntoScene, this, std::placeholders::_1, std::placeholders::_2));
	
	
}


void GatewayServer::OnRequestZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RequestZoneRoleData>& pRecv) {
	ForwardToData(1, pRecv);
}


void GatewayServer::OnResponseZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ResponseZoneRoleData>& pRecv) {
	ForwardToLogin(pRecv->svr_id(), pRecv);
}
void GatewayServer::OnReqCreateRole(const TcpConnectionPtr&, const std::shared_ptr<ReqCreateRole>& pRecv) {
	ForwardToData(1, pRecv);
}

void GatewayServer::OnRspCreateRole(const TcpConnectionPtr&, const std::shared_ptr<RspCreateRole>& pRecv) {
	ForwardToLogin(pRecv->svr_id(), pRecv);
}
void GatewayServer::OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>& pRecv) {
	ForwardToData(1, pRecv);
}
void GatewayServer::OnLoginIntoScene(const TcpConnectionPtr&, const std::shared_ptr<LoginIntoScene>& pRecv) {
	ForwardToScene(1, pRecv);

	auto pSend = std::make_shared<RspEnterGame>();
	pSend->set_act_id(pRecv->act_id());
	pSend->set_role_id(pRecv->role_id());
	pSend->set_result(1); // fix: into scene
	ForwardToLogin(1, pSend);
}

void GatewayServer::ForwardToLogin(unsigned int svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (unsigned int)ServerType::LOGIN, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to login, msg = " << pMsg->GetTypeName();
}

void GatewayServer::ForwardToData(unsigned int svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (unsigned int)ServerType::DATA, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to data, msg = " << pMsg->GetTypeName();
}

void GatewayServer::ForwardToScene(unsigned int svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (unsigned int)ServerType::SCENE, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to scene, msg = " << pMsg->GetTypeName();
}

int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	GatewayServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}