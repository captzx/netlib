#include "DBConnection.h"

#include "Logger.h"

using namespace x::tool;

/// DBConnection
DBConnection::DBConnection(const std::string& url) : 
	_pSession(std::make_unique<Session>(url)) {
	
	log(debug, "DBConnection") << "db connection success. url: " << url;
}

SqlResult DBConnection::ExecuteSql(const std::string& statement) {
	SqlResult result = _pSession->sql(statement).execute();

	log(debug, "DBConnection") << "execute sql: " << statement;
	if (result.hasData()) {
		log(debug, "DBConnection") << "column count: " << result.getColumnCount();
		log(debug, "DBConnection") << "row count: " << result.count();
		log(debug, "DBConnection") << "affected count: " << result.getAffectedItemsCount();
		log(debug, "DBConnection") << "warnning count: " << result.getWarningsCount();
	}
	else {
		log(debug, "DBConnection") << "has data: 0.";
	}

	return result;
}

/// DBService
DBService::DBService() {

}

void DBService::Connect(uint32_t type, std::string url) {
	auto it = _pDBConnections.find(type);
	if (it == _pDBConnections.end()) {
		_pDBConnections[type] = std::make_shared<DBConnection>(url);
	}
}

DBConnectionPtr DBService::GetDBConnectionByType(uint32_t type) {
	auto it = _pDBConnections.find(type);
	if (it != _pDBConnections.end()) return it->second;

	return nullptr;
}