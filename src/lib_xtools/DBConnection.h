#pragma once

#include <string>
#include <memory>

#include <mysqlx/xdevapi.h>

#include "NoCopyable.h"

using mysqlx::Session;

namespace x {

namespace tool {

class DBConnection :public NoCopyable {
public:
	explicit DBConnection(const std::string&);

public:
	void Init();
	void CreateSchema(const std::string&);
	void CreateTable(const std::string&);
	void ExecuteSql(const std::string&);

private:
	std::shared_ptr<Session> _pSession; // fix: unique_ptr
};
using DBConnectionPtr = std::shared_ptr<DBConnection>;

}

}