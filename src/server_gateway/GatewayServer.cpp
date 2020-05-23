#include "GatewayServer.h"

using namespace x::gateway;

/// GatewayServer
GatewayServer::GatewayServer():
	_dispatcher(std::bind(&GatewayServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

}

void GatewayServer::Start() {
	_pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType(ServerType::GATEWAY);
	if (!_pSvrCfg) {
		std::cout << "start GatewayServer failure, error code: config missing.";
		return;
	}

	global_logger_init(_pSvrCfg->LogFile);
	global_logger_set_filter(severity >= (severity_level)_pSvrCfg->LogLevel);

	_pTcpService = std::make_shared<TcpService>(_pSvrCfg->Name);

	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&GatewayServer::OnConnection, this, std::placeholders::_1));

	_pTcpService->AsyncListen(_pSvrCfg->Port);
	_pTcpService->Start();

	log(debug, "GatewayServer") << "GatewayServer start.";

	while (1); // fix
}

void GatewayServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "GatewayServer") << "GatewayServer default message call back.";
}

void GatewayServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "GatewayServer") << "on Connection.";
}