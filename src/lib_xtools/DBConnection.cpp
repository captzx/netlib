#include "DBConnection.h"

#include "Logger.h"

using namespace mysqlx;
using namespace x::tool;

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