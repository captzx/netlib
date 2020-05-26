#include "DataServer.h"

using namespace x::data;
using namespace mysqlx;

/// DataServer
DataServer::DataServer():_rolecount(0) {

}

void DataServer::InitModule() {
	ProtobufDispatcher& dispatcher = DataServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestZoneRoleData>(std::bind(&DataServer::OnRequestZoneRoleData, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqCreateRole>(std::bind(&DataServer::OnReqCreateRole, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<ReqEnterGame>(std::bind(&DataServer::OnReqEnterGame, this, std::placeholders::_1, std::placeholders::_2));
	

	std::ostringstream oss;

	oss << "SELECT global.`value` FROM x_data.global  WHERE global.`var` = 'rolecount';";

	DBServicePtr& pDBService = DataServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());
	if (result.count() != 0) {
		Row row = result.fetchOne();
		assert(!row.isNull());
		_rolecount = static_cast<uint32_t>(row.get(0));
	}
}

void DataServer::OnRequestZoneRoleData(const TcpConnectionPtr& pConn, const std::shared_ptr<RequestZoneRoleData>& pRecv) {
	std::ostringstream oss;

	oss << "SELECT * FROM x_data.role WHERE role.`actid` = '" << pRecv->act_id() << "';";

	DBServicePtr& pDBService = DataServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());
	
	auto pSend = std::make_shared<ResponseZoneRoleData>();
	pSend->set_svr_id(pRecv->svr_id()); // 1
	pSend->set_act_id(pRecv->act_id());

	if (result.count() == 0) {
		log(debug, "DataServer") << "player data == null, new player please!";
	}
	else {
		RoleList* pRolelist = pSend->mutable_rolelist();

		log(debug, "DataServer") << "result.count() = " << result.count();
		
		Row row = result.fetchOne();
		while (!row.isNull()) { // vs. for(int i = 0; i < result.count(); ++i)
			// ull accid = static_cast<uint64_t>(row.get(0));
			ull roleid = static_cast<uint64_t>(row.get(1));
			std::string name = static_cast<std::string>(row.get(2));

			RoleInfo* pRole = pRolelist->add_roles();
			pRole->set_role_id(roleid);
			pRole->set_name(name);

			log(debug, "DataServer") << "rolelist, roleid = " << roleid << ", name = " << name;

			row = result.fetchOne();
		}
	}
	pConn->AsyncSend(pSend);
}

void DataServer::OnReqCreateRole(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqCreateRole>& pRecv) {
	unsigned int roleid = 1 << 31 | pRecv->zone_id() << 16 | _rolecount;
	log(debug, "DataServer") << "create role id: " << roleid;

	unsigned int result = 1;

	std::shared_ptr<RspCreateRole> pSend = std::make_shared<RspCreateRole>();
	if (pSend) {
		pSend->set_result(result);

		pSend->set_act_id(pRecv->act_id());
		pSend->set_svr_id(pRecv->svr_id());

		RoleInfo* pRole = pSend->mutable_role();
		pRole->set_role_id(roleid);
		pRole->set_name(pRecv->name());
		
		pConn->AsyncSend(pSend);
	}

	if (result == 1) {
		
		++_rolecount;

		DBServicePtr& pDBService = DataServer::GetInstance().GetDBService();

		// updata rolecount
		std::string sql = "select * from  x_data.global where `var` = 'rolecount';";
		SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(sql);
		if (result.count() == 0) {
			std::ostringstream oss;
			oss << "INSERT INTO x_data.global(`var`, `value`) VALUES ('rolecount', '" << _rolecount << "');";
			result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());
		}
		else {
			std::ostringstream oss;
			oss << "UPDATE x_data.global SET `value` = '" << _rolecount << "'  WHERE `var` = 'rolecount';";
			result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());
		}

		// save role
		std::ostringstream oss;
		oss << "INSERT INTO x_data.role(`actid`, `roleid`, `name`) VALUES ('" << pRecv->act_id() << "', '" << roleid << "', '" << pRecv->name() << "');";
		result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());

		log(debug, "DataServer") << "Update _rolecount, value = " << _rolecount;
	}

}


void DataServer::OnReqEnterGame(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqEnterGame>& pRecv) {
	std::ostringstream oss;

	oss << "SELECT * FROM x_data.role WHERE role.`roleid` = '" << pRecv->role_id() << "';";

	DBServicePtr& pDBService = DataServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());

	assert(result.hasData());

	Row row = result.fetchOne();
	ull actid = static_cast<uint64_t>(row.get(0));
	ull roleid = static_cast<uint64_t>(row.get(1));
	std::string name = static_cast<std::string>(row.get(2));

	auto pSend = std::make_shared<LoginIntoScene>();
	pSend->set_act_id(actid);
	pSend->set_role_id(roleid);
	pSend->set_name(name);
	pConn->AsyncSend(pSend);
}

int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	DataServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}