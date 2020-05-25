#pragma once

#include <string>
#include <memory>

#include <mysqlx/xdevapi.h>

#include "NoCopyable.h"

using mysqlx::Session;
using mysqlx::SqlResult;

namespace x {

namespace tool {

enum class DBType : unsigned int {
	x_login = 1,
	x_data = 2,
};
class DBConnection :public NoCopyable {
public:
	explicit DBConnection(const std::string&);

public:
	void Init();
	SqlResult ExecuteSql(const std::string&);

private:
	std::shared_ptr<Session> _pSession; // fix: unique_ptr
};
using DBConnectionPtr = std::shared_ptr<DBConnection>;

class DBService {
public:
	DBService();

public:
	void Connect(unsigned int type, std::string url);
	DBConnectionPtr GetDBConnectionByType(unsigned int);

private:
	std::map<unsigned int, DBConnectionPtr> _pDBConnections;
};
using DBServicePtr = std::shared_ptr<DBService>;

}

}