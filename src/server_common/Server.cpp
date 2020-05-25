#include "Server.h"

using namespace x;

/// Server
Server::Server() :
	_dispatcher(std::bind(&Server::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {
	_heartbeatPeriod = 3;
	_dispatcher.RegisterMessageCallback<ActiveHeartBeat>(std::bind(&Server::OnActiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback<PassiveHeartBeat>(std::bind(&Server::OnPassiveHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback<ActiveConnection>(std::bind(&Server::OnActiveConnection, this, std::placeholders::_1, std::placeholders::_2));
}

void Server::Start(unsigned int id) {
	_pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType({ static_cast<unsigned int>(GetServerType()), id });
	if (!_pSvrCfg) {
		std::cout << "start Server failure, error code: config missing.";
		return;
	}

	global_logger_init(_pSvrCfg->LogFile);
	global_logger_set_filter(severity >= (severity_level)_pSvrCfg->LogLevel);

	_pDBService = std::make_shared<DBService>();
	for (auto db : _pSvrCfg->ConnectDBCfgs) {
		_pDBService->Connect(db.Type, db.Url);
	}

	_pTcpService = std::make_shared<TcpService>(_pSvrCfg->Name);
	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetAtvConnCallback(std::bind(&Server::OnAtvConnection, this, std::placeholders::_1));
	_pTcpService->SetPsvConnCallback(std::bind(&Server::OnPsvConnection, this, std::placeholders::_1));
	for (auto svr : _pSvrCfg->ConnectSvrCfgs) {
		auto pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType({ svr.Type, svr.ID });
		if (pSvrCfg) _connections[{svr.Type, svr.ID}] = _pTcpService->AsyncConnect(pSvrCfg->IP, pSvrCfg->Port);
	}

	InitModule(); // another module

	_pTcpService->AsyncListen(_pSvrCfg->Port);
	_pTcpService->Start();

	log(debug, _pSvrCfg->Name) << "Server start.";

	_startTime = Now::Second();

	while (1); // fix
}

void Server::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, _pSvrCfg->Name) << "Server default message call back.";
}

void Server::OnAtvConnection(const TcpConnectionPtr& pConnection) {
	auto pMsg = std::make_shared<ActiveConnection>();
	if (pMsg) {
		pMsg->set_type(_pSvrCfg->Type);
		pMsg->set_id(_pSvrCfg->ID);

		pConnection->AsyncSend(pMsg);
	}
}
void Server::OnPsvConnection(const TcpConnectionPtr&) {
	log(debug, _pSvrCfg->Name) << "on Connection.";
}
void Server::CheckHeartBeat() {
	unsigned int now = Now::Second();
	TcpConnectionManager& passiveConnections = _pTcpService->GetPassiveConnectionMgr();
	for (auto it = passiveConnections.begin(); it != passiveConnections.end();) {
		const TcpConnectionPtr& pConnection = it->second;
		if (pConnection) {
			unsigned int last = pConnection->GetLastHeartBeatTime();
			if (now > last + _heartbeatPeriod * 2) {
				if (pConnection->IsConnectioned()) {
					pConnection->Disconnect();
				}
			}
			else {
				auto pMsg = std::make_shared<ActiveHeartBeat>();
				if (pMsg) {
					pMsg->set_last_hb_time(now);
					pConnection->AsyncSend(pMsg);
				}

				++it;
			}
		}
	}
}


void Server::OnPassiveHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<PassiveHeartBeat>& pMsg) {
	pConnection->SetLastHeartBeatTime(pMsg->last_hb_time());

	log(trivial, _pSvrCfg->Name) << pMsg->address() << ":" << pMsg->cnt_start_time() << ":" << pMsg->pid()
		<< " active count: " << pMsg->atv_count() << " passive count: " << pMsg->psv_count();
}

void Server::OnActiveHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<ActiveHeartBeat>& pMsg) {
	pConnection->SetLastHeartBeatTime(pMsg->last_hb_time());

	CheckHeartBeat();

	if (pConnection->IsConnectioned()) {
		auto pMsg = std::make_shared<PassiveHeartBeat>();
		if (pMsg) {
			pMsg->set_last_hb_time(pConnection->GetLastHeartBeatTime());
			pMsg->set_cnt_start_time(pConnection->GetStartTime());
			pMsg->set_address(pConnection->GetLocalEndpoint());
			pMsg->set_pid(pConnection->GetPid());
			pMsg->set_atv_count(_pTcpService->GetActiveConnectCount());
			pMsg->set_psv_count(_pTcpService->GetPassiveConnectCount());

			pConnection->AsyncSend(pMsg);
		}
	}
}

void Server::OnActiveConnection(const TcpConnectionPtr& pConnection, const std::shared_ptr<ActiveConnection>& pMsg) {
	_connections[{pMsg->type(), pMsg->id()}] = pConnection;

	log(debug, _pSvrCfg->Name) << "OnActiveConnection {pMsg, svr.ID}: " << pMsg->type() << " " << pMsg->id();
}
