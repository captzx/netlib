#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
GatewayServer::GatewayServer(){

}

void GatewayServer::InitModule() {
	GET_MESSAGE_DISPATCHER(GatewayServer::GetInstance().GetDispatcher());
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, ReqZoneRoleData);
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, RspZoneRoleData);
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, ReqCreateRole);
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, RspCreateRole);
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, ReqEnterGame);
	REGISTER_MESSAGE_CALL_BACK(GatewayServer, this, LoginIntoScene);
}


void GatewayServer::OnReqZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ReqZoneRoleData>& pRecv) {
	ForwardToData(1, pRecv);
}


void GatewayServer::OnRspZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RspZoneRoleData>& pRecv) {
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

void GatewayServer::ForwardToLogin(uint32_t svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (uint32_t)ServerType::LOGIN, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to login, msg = " << pMsg->GetTypeName();
}

void GatewayServer::ForwardToData(uint32_t svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (uint32_t)ServerType::DATA, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to data, msg = " << pMsg->GetTypeName();
}

void GatewayServer::ForwardToScene(uint32_t svr_id, const MessagePtr& pMsg) {
	auto it = _connections.find({ (uint32_t)ServerType::SCENE, svr_id });
	if (it != _connections.end() && !it->second.expired()) {
		TcpConnectionPtr pConnection = it->second.lock();
		pConnection->AsyncSend(pMsg);
	}

	log(debug, "GatewayServer") << "forward message to scene, msg = " << pMsg->GetTypeName();
}

int32_t main(int32_t argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	GatewayServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}