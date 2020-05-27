#include "Server.h"

using namespace x;

/// Server
Server::Server() :
	_dispatcher(std::bind(&Server::DefaultMessageCallback, this, _1, _2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, _1, _2)) {
	_heartbeatPeriod = 3;

	GET_MESSAGE_DISPATCHER(_dispatcher)
	REGISTER_MESSAGE_CALL_BACK(Server, this, ActiveHeartBeat);
	REGISTER_MESSAGE_CALL_BACK(Server, this, PassiveHeartBeat);
	REGISTER_MESSAGE_CALL_BACK(Server, this, ActiveConnection);
}

void Server::Start(uint32_t id) {
	_pSvrCfg = GlobalConfig::GetInstance().GetServerCfgByType({ static_cast<uint32_t>(GetServerType()), id });
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
	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, _1, _2));
	_pTcpService->SetAtvConnCallback(std::bind(&Server::OnAtvConnection, this, _1));
	_pTcpService->SetPsvConnCallback(std::bind(&Server::OnPsvConnection, this, _1));
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

void Server::OnAtvConnection(const TcpConnectionPtr& pConn) {
	auto pSend = std::make_shared<ActiveConnection>();
	if (pSend) {
		pSend->set_type(_pSvrCfg->Type);
		pSend->set_id(_pSvrCfg->ID);

		pConn->AsyncSend(pSend);
	}
}
void Server::OnPsvConnection(const TcpConnectionPtr&) {
	log(debug, _pSvrCfg->Name) << "on Connection.";
}
void Server::CheckHeartBeat() {
	uint32_t now = Now::Second();

	std::map<int32_t, TcpConnectionPtr>& psvConns = _pTcpService->GetPsvConnMgr().GetAll();
	for (auto it = psvConns.begin(); it != psvConns.end();) {
		const TcpConnectionPtr& pConn = it->second;
		if (pConn) {
			uint32_t last = pConn->GetLastHeartBeatTime();
			if (now > last + _heartbeatPeriod * 2) {
				if (pConn->IsConnectioned()) {
					pConn->Disconnect();
				}
			}
			else {
				auto pSend = std::make_shared<ActiveHeartBeat>();
				if (pSend) {
					pSend->set_last_hb_time(now);
					pConn->AsyncSend(pSend);
				}

				++it;
			}
		}
	}
}


void Server::OnPassiveHeartBeat(const TcpConnectionPtr& pConn, const std::shared_ptr<PassiveHeartBeat>& pRecv) {
	pConn->SetLastHeartBeatTime(pRecv->last_hb_time());

	log(trivial, _pSvrCfg->Name) << pRecv->address() << ":" << pRecv->cnt_start_time() << ":" << pRecv->pid()
		<< " active count: " << pRecv->atv_count() << " passive count: " << pRecv->psv_count();
}

void Server::OnActiveHeartBeat(const TcpConnectionPtr& pConn, const std::shared_ptr<ActiveHeartBeat>& pRecv) {
	pConn->SetLastHeartBeatTime(pRecv->last_hb_time());

	CheckHeartBeat();

	if (pConn->IsConnectioned()) {
		auto pSend = std::make_shared<PassiveHeartBeat>();
		if (pSend) {
			pSend->set_last_hb_time(pConn->GetLastHeartBeatTime());
			pSend->set_cnt_start_time(pConn->GetStartTime());
			pSend->set_address(pConn->GetLocalEndpoint());
			pSend->set_pid(pConn->GetPid());
			pSend->set_atv_count(_pTcpService->GetAtvConnMgr().GetConnectedCount());
			pSend->set_psv_count(_pTcpService->GetPsvConnMgr().GetConnectedCount());

			pConn->AsyncSend(pSend);
		}
	}
}

void Server::OnActiveConnection(const TcpConnectionPtr& pConn, const std::shared_ptr<ActiveConnection>& pRecv) {
	_connections[{pRecv->type(), pRecv->id()}] = pConn;

	log(debug, _pSvrCfg->Name) << "OnActiveConnection {pRecv, svr.ID}: " << pRecv->type() << " " << pRecv->id();
}
