#include "ZoneServerManager.h"

#include "LoginUserManager.h"

using namespace x::net;
using namespace x::tool;

using namespace x::login;
using namespace mysqlx;

void ZoneServerManager::RegisterMessageCallback() {
	ProtobufDispatcher& dispatcher = LoginServer::GetInstance().GetDispatcher();

	dispatcher.RegisterMessageCallback<SelectZoneServer>(std::bind(&ZoneServerManager::OnSelectZoneServer, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ResponseZoneRoleData>(std::bind(&ZoneServerManager::OnResponseZoneRoleData, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqCreateRole>(std::bind(&ZoneServerManager::OnReqCreateRole, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RspCreateRole>(std::bind(&ZoneServerManager::OnRspCreateRole, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqEnterGame>(std::bind(&ZoneServerManager::OnReqEnterGame, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RspEnterGame>(std::bind(&ZoneServerManager::OnRspEnterGame, this, std::placeholders::_1, std::placeholders::_2));
	
	
}

bool ZoneServerManager::InitServerList() {
	std::ostringstream oss;

	oss << "select * from x_login.serverlist;";

	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_login)->ExecuteSql(oss.str());
	for (size_t i = 0; i < result.count(); ++i) {
		Row row = result.fetchOne();
		assert(!row.isNull());

		ZoneServer zone;
		zone.id = static_cast<int>(row.get(0));
		zone.name = static_cast<std::string>(row.get(1));
		zone.ip = static_cast<std::string>(row.get(2));
		zone.port = static_cast<unsigned int>(row.get(3));
		zone.state = static_cast<unsigned int>(row.get(4));

		zone.wpConnection = LoginServer::GetInstance().GetTcpService()->AsyncConnect(zone.ip, zone.port); // connect gateway

		_zoneList[zone.id] = zone;

		log(debug, "ZoneServerManager") << "load zoneserver, name£º " << zone.name << ", ip: " << zone.ip
			<< ", port:" << zone.port << ", state:" << zone.state << ".";
	}

	if (_zoneList.size() == 0) log(warning, "ZoneServerManager") << "zoneserver list == null";

	return true;
}

bool ZoneServerManager::SendAllZoneList(const TcpConnectionPtr& pConnection) {
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

		pConnection->AsyncSend(pMsg);
	}

	return true;
	
}

void ZoneServerManager::OnResponseZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ResponseZoneRoleData>& pRecv) {
	std::shared_ptr<LoginUser> pUser = LoginUserManager::GetInstance().FindUser(pRecv->act_id());
	if (pUser && !pUser->GetConnection().expired()) {
		TcpConnectionPtr pConnection = pUser->GetConnection().lock();
		RoleList rolelist = pRecv->rolelist();
		std::shared_ptr<RoleList> pSend = std::make_shared<RoleList>(rolelist);
		pConnection->AsyncSend(pSend);
	}
	log(warning, "ZoneServerManager") << "OnResponseZoneRoleData";
}
void ZoneServerManager::OnSelectZoneServer(const TcpConnectionPtr& pConnection, const std::shared_ptr<SelectZoneServer>& pRecv) {
	auto it = _zoneList.find(pRecv->zone_id());
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		auto pSend = std::make_shared<RequestZoneRoleData>();
		if (pSend) {
			pSend->set_svr_id(1); // 1
			pSend->set_act_id(pRecv->act_id());

			pToGateWay->AsyncSend(pSend);

			log(warning, "ZoneServerManager") << "async send RequestZoneRoleData, act_id = " << pSend->act_id();
		}
	}
}

void ZoneServerManager::OnReqCreateRole(const TcpConnectionPtr& pConnection, const std::shared_ptr<ReqCreateRole>& pRecv) {
	auto it = _zoneList.find(pRecv->zone_id());
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		
		pRecv->set_svr_id(1); // server id
		pToGateWay->AsyncSend(pRecv);
	}
}

void ZoneServerManager::OnRspCreateRole(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspCreateRole>& pRecv) {
	std::shared_ptr<LoginUser> pUser = LoginUserManager::GetInstance().FindUser(pRecv->act_id());
	if (pUser && !pUser->GetConnection().expired()) {
		TcpConnectionPtr pConnection = pUser->GetConnection().lock();
		pConnection->AsyncSend(pRecv);
	}
}


void ZoneServerManager::OnReqEnterGame(const TcpConnectionPtr& pConnection, const std::shared_ptr<ReqEnterGame>& pRecv) {
	auto it = _zoneList.find(1);
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		pToGateWay->AsyncSend(pRecv);
	}
}

void ZoneServerManager::OnRspEnterGame(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspEnterGame>& pRecv) {
	log(warning, "ZoneServerManager") << "todo: OnRspEnterGame";
}