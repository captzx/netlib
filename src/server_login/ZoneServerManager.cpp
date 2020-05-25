#include "ZoneServerManager.h"

using namespace x::net;
using namespace x::tool;

using namespace x::login;
using namespace mysqlx;

void ZoneServerManager::RegisterMessageCallback() {
	ProtobufDispatcher& dispatcher = LoginServer::GetInstance().GetDispatcher();

	dispatcher.RegisterMessageCallback<SelectZoneServer>(std::bind(&ZoneServerManager::OnSelectZoneServer, this, std::placeholders::_1, std::placeholders::_2));
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

void ZoneServerManager::OnSelectZoneServer(const TcpConnectionPtr& pConnection, const std::shared_ptr<SelectZoneServer>& pRecv) {
	auto it = _zoneList.find(pRecv->zone_id());
	if (it != _zoneList.end() && !it->second.wpConnection.expired()) {
		TcpConnectionPtr pToGateWay = it->second.wpConnection.lock();
		auto pSend = std::make_shared<RequestPlayerLoginData>();
		if (pSend) {
			pSend->set_act_id(pRecv->act_id());

			pToGateWay->AsyncSend(pSend);

			log(warning, "ZoneServerManager") << "async send RequestPlayerLoginData, act_id = " << pSend->act_id();
		}
	}
}
