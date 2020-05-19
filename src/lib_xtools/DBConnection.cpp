#include "DBConnection.h"

#include "Logger.h"

using namespace mysqlx;
using namespace x::tool;

DBConnection::DBConnection(const std::string& url){
	_pSession = std::make_shared<Session>(url);
	log(debug, "DBConnection") << "db connection success.";

	Init();
}

void DBConnection::Init() {
	CreateSchema("create database if not exists test2");
}

void DBConnection::CreateSchema(const std::string& statement) {
	// R"(create database if not exists test2)"
	_pSession->sql(statement).execute();

	 log(debug, "DBConnection") << "todo: CreateSchema process SqlResult";
}

void DBConnection::CreateTable(const std::string& statement) {
	_pSession->sql(statement).execute();

	log(debug, "DBConnection") << "todo: CreateTable process SqlResult";
}

void DBConnection::ExecuteSql(const std::string& statement) {
	// SqlResult result = 
	_pSession->sql(statement).execute();

	log(debug, "DBConnection") << "todo: ExecuteSql process SqlResult";
}