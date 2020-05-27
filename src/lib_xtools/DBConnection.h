#pragma once

#include <string>
#include <memory>

#include <mysqlx/xdevapi.h>
using namespace mysqlx;

#include "NoCopyable.h"

namespace x {

namespace tool {

enum class DBType : uint32_t {
	x_login = 1,
	x_data = 2,
};

/// DBConnection
class DBConnection :public NoCopyable {
public:
	explicit DBConnection(const std::string&);

public:
	SqlResult ExecuteSql(const std::string&);

private:
	std::unique_ptr<Session> _pSession; // fix: unique_ptr
};
using DBConnectionPtr = std::shared_ptr<DBConnection>;

/// DBService
class DBService {
public:
	DBService();

public:
	void Connect(uint32_t type, std::string url);
	DBConnectionPtr GetDBConnectionByType(uint32_t);

private:
	std::map<uint32_t, DBConnectionPtr> _pDBConnections;
};
using DBServicePtr = std::shared_ptr<DBService>;

}

}