#pragma once

#include <string>

#include <xtools/Singleton.h>
#include <xtools/Config.h>

using x::tool::Config;
using x::tool::Singleton;

namespace x {

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

} // x