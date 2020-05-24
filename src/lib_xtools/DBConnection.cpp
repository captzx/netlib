#include "DBConnection.h"

#include "Logger.h"

using namespace mysqlx;
using namespace x::tool;

/// DBConnection
DBConnection::DBConnection(const std::string& url){
	_pSession = std::make_shared<Session>(url);
	log(debug, "DBConnection") << "db connection success. url: " << url;

	Init();
}

void DBConnection::Init() {
	
}

SqlResult DBConnection::ExecuteSql(const std::string& statement) {
	SqlResult result = _pSession->sql(statement).execute();

	log(debug, "DBConnection") << "execute sql:" << statement;

	log(debug, "DBConnection") << "todo: ExecuteSql process SqlResult";

	return result;
}

/// DBService
DBService::DBService() {

}

void DBService::Connect(unsigned int type, std::string url) {
	auto it = _pDBConnections.find(type);
	if (it == _pDBConnections.end()) {
		_pDBConnections[type] = std::make_shared<DBConnection>(url);
	}
}

DBConnectionPtr DBService::GetDBConnectionByType(unsigned int type) {
	auto it = _pDBConnections.find(type);
	if (it != _pDBConnections.end()) return it->second;

	return nullptr;
}