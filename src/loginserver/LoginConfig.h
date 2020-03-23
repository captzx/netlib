#pragma once

#include <string>

#include "Singleton.h"
#include "Config.h"

using piece::tool::Config;
using piece::tool::Singleton;

namespace piece {

namespace login {

class LoginConfig : public Config, public Singleton<LoginConfig>{
public:
	virtual bool Parse() override;

public:
	unsigned int GetListenPort() { return _port; }
	const std::string& GetLogFile() { return _logfile; }

private:
	std::string _logfile;
	unsigned int _port;
};

} // login

} // piece