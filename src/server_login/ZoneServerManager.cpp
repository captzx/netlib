#include "ZoneServerManager.h"

#include "LoginUserManager.h"

using namespace x::login;

void ZoneServerManager::RegisterMessageCallback() {
	GET_MESSAGE_DISPATCHER(LoginServer::GetInstance().GetDispatcher());
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, SelectZoneServer);
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, RspZoneRoleData);
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, ReqCreateRole);
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, RspCreateRole);
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, ReqEnterGame);
	REGISTER_MESSAGE_CALL_BACK(ZoneServerManager, this, RspEnterGame);
}

bool ZoneServerManager::InitServerList() {
	std::ostringstream oss;

	oss << "select * from x_login.serverlist;";

	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((uint32_t)DBType::x_login)->ExecuteSql(oss.str());
	for (size_t i = 0; i < result.count(); ++i) {
		Row row = result.fetchOne();
		assert(!row.isNull());

		ZoneServer zone;
		zone.id = static_cast<int32_t>(row.get(0));
		zone.name = static_cast<std::string>(row.get(1));
		zone.ip = static_cast<std::string>(row.get(2));
		zone.port = static_cast<uint32_t>(row.get(3));
		zone.state = static_cast<uint32_t>(row.get(4));

		zone.wpConnection = LoginServer::GetInstance().GetTcpService()->AsyncConnect(zone.ip, zone.port); // connect gateway

		_zoneList[zone.id] = zone;

		log(debug, "ZoneServerManager") << "load zoneserver, name£º " << zone.name << ", ip: " << zone.ip
			<< ", port:" << zone.port << ", state:" << zone.state << ".";
	}

	if (_zoneList.size() == 0) log(warning, "ZoneServerManager") << "zoneserver list == null";

	return true;
}

bool ZoneServerManager::SendAllZoneList(const TcpConnectionPtr& pConn) {
	auto pMsg = std::make_shared<ReturnZoneList>();
	if (pMsg) {
		for (auto it : _zoneList) {
			ZoneServer zone = it.second;

			auto pZoneServer = pMsg->add_zone_servers();
			if (pZoneServer) {
				pZoneServer->set_id(zone.id);
				pZoneServer->set_ip(zone.ip);
				pZoneServer->set_port(zone.port);
				pZoneServer->set_name(zone.name);
			}
		}

		pConn->AsyncSend(pMsg);
	}

	return true;
	
}

void ZoneServerManager::OnRspZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RspZoneRoleData>& pRecv) {
	std::shared_ptr<LoginUser> pUser = LoginUserManager::GetInstance().FindUser(pRecv->act_id());
	if (pUser && !pUser->GetConnection().expired()) {
		TcpConnectionPtr pConn = pUser->GetConnection().lock();

		auto address = pUser->GetZoneAddress();
		pRecv->set_ip(address.first);
		pRecv->set_port(address.second);
		pConn->AsyncSend(pRecv);
	}
	log(warning, "ZoneServerManager") << "OnRspZoneRoleData";
}
void ZoneServerManager::OnSelectZoneServer(const TcpConnectionPtr& pConn, const std::shared_ptr<SelectZoneServer>& pRecv) {
	auto it = _zoneList.find(pRecv->zone_id());
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		auto pSend = std::make_shared<ReqZoneRoleData>();
		if (pSend) {
			pSend->set_svr_id(1); // 1
			pSend->set_act_id(pRecv->act_id());

			pToGateWay->AsyncSend(pSend);

			log(warning, "ZoneServerManager") << "async send ReqZoneRoleData, act_id = " << pSend->act_id();
		}

		std::shared_ptr<LoginUser> pUser = LoginUserManager::GetInstance().FindUser(pRecv->act_id());
		pUser->SetZoneAddress({ it->second.ip, it->second.port });
	}
}

void ZoneServerManager::OnReqCreateRole(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqCreateRole>& pRecv) {
	auto it = _zoneList.find(pRecv->zone_id());
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		
		pRecv->set_svr_id(1); // server id
		pToGateWay->AsyncSend(pRecv);
	}
}

void ZoneServerManager::OnRspCreateRole(const TcpConnectionPtr& pConn, const std::shared_ptr<RspCreateRole>& pRecv) {
	std::shared_ptr<LoginUser> pUser = LoginUserManager::GetInstance().FindUser(pRecv->act_id());
	if (pUser && !pUser->GetConnection().expired()) {
		TcpConnectionPtr pConn = pUser->GetConnection().lock();
		pConn->AsyncSend(pRecv);
	}
}


void ZoneServerManager::OnReqEnterGame(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqEnterGame>& pRecv) {
	auto it = _zoneList.find(1);
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		pToGateWay->AsyncSend(pRecv);
	}
}

void ZoneServerManager::OnRspEnterGame(const TcpConnectionPtr& pConn, const std::shared_ptr<RspEnterGame>& pRecv) {
	log(warning, "ZoneServerManager") << "todo: OnRspEnterGame";
}