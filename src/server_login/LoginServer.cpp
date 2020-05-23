#include "LoginServer.h"

#include "LoginUserManager.h"

using namespace x::login;

/// LoginServer
LoginServer::LoginServer():
	_dispatcher(std::bind(&LoginServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {
	
	_dispatcher.RegisterMessageCallback<HeartBeat>(std::bind(&LoginServer::OnHeartBeat, this, std::placeholders::_1, std::placeholders::_2));

}

void LoginServer::Start() {
	_pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType(ServerType::LOGIN);
	if (!_pSvrCfg) {
		std::cout << "start LoginServer failure, error code: config missing.";
		return;
	}

	global_logger_init(_pSvrCfg->LogFile);
	global_logger_set_filter(severity >= (severity_level)_pSvrCfg->LogLevel);

	_pTcpService = std::make_shared<TcpService>(_pSvrCfg->Name);
	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&LoginServer::OnConnection, this, std::placeholders::_1));

	for (auto connectDB : _pSvrCfg->ConnectDBCfgs) {
		if (connectDB.Type == 1) {
			_pDBLoginConnection = std::make_shared<DBConnection>(connectDB.Url);
		}
	}

	for (auto connectSvr : _pSvrCfg->ConnectSvrCfgs) {
		if (connectSvr.Type == (unsigned int)ServerType::GATEWAY) {
			_pGateWayConnection = _pTcpService->AsyncConnect(connectSvr.IP, connectSvr.Port);
		}
	}

	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback(_dispatcher);

	_pTcpService->AsyncListen(_pSvrCfg->Port);
	_pTcpService->Start();

	log(debug, "LoginServer") << "LoginServer start.";

	while (1); // fix
}

void LoginServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "LoginServer") << "LoginServer default message call back.";
}

void LoginServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "LoginServer") << "on Connection.";
}

void LoginServer::OnHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<HeartBeat>& pMessage) {
	pConnection->SetLastHeartBeatTime(pMessage->time());
}

bool LoginServer::SendHeartBeat(const TcpConnectionPtr& pConnection) {
	auto pMsg = std::make_shared<HeartBeat>();
	if (pMsg) {
		unsigned int now = Now::Second();
		pMsg->set_time(now);
		pConnection->AsyncSend(pMsg);
	}

	return true;
}
