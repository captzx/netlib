#include "SupervisorServer.h"

using namespace x::supervisor;

/// SupervisorServer
SupervisorServer::SupervisorServer():
	_dispatcher(std::bind(&SupervisorServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

}

void SupervisorServer::Start() {
	_pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType(ServerType::SUPERVISOR);
	if (!_pSvrCfg) {
		std::cout << "start SupervisorServer failure, error code: config missing.";
		return;
	}

	global_logger_init(_pSvrCfg->LogFile);
	global_logger_set_filter(severity >= (severity_level)_pSvrCfg->LogLevel);

	_pTcpService = std::make_shared<TcpService>(_pSvrCfg->Name);

	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&SupervisorServer::OnConnection, this, std::placeholders::_1));

	_pTcpService->AsyncListen(_pSvrCfg->Port);
	_pTcpService->Start();

	log(debug, "SupervisorServer") << "SupervisorServer start.";

	while (1); // fix
}

void SupervisorServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "SupervisorServer") << "SupervisorServer default message call back.";
}

void SupervisorServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "SupervisorServer") << "on Connection.";
}