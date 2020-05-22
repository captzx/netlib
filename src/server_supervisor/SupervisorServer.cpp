#include "SupervisorServer.h"

using namespace x::supervisor;
using namespace x::tool;

/// SupervisorServer
SupervisorServer::SupervisorServer(std::string name):
	_dispatcher(std::bind(&SupervisorServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

	_pTcpService = std::make_shared<TcpService>(name);

	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&SupervisorServer::OnConnection, this, std::placeholders::_1));
}

void SupervisorServer::Start() {
	GatewayConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(GatewayConfig::GetInstance().GetLogFile());
	global_logger_set_filter(severity >= trivial);
	_pTcpService->AsyncListen(GatewayConfig::GetInstance().GetListenPort());
	_pTcpService->Start();
	log(debug, "SupervisorServer") << "gateway server start.";
}

void SupervisorServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "SupervisorServer") << "login server default message call back.";
}

void SupervisorServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "SupervisorServer") << "on Connection.";
}