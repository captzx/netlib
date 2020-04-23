#include "common.h"
#include "LoginServer.h"

#include <xnet/NetServer.h>

using namespace x::login;
using namespace x::tool;

/// LoginServer
LoginServer::LoginServer(std::string name){
	_pTcpServer = std::make_shared<NetServer>(name);
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(LoginConfig::GetInstance().GetLogFile());
	_pTcpServer->Listen(LoginConfig::GetInstance().GetListenPort());
}

/// LoginConfig
bool LoginConfig::Parse() {
	const std::shared_ptr<XMLDocument>& pDoc = GetXmlDoc();
	if (!pDoc) {
		std::cout << "xml document not found, is file exist?" << std::endl;
		return false;
	}

	XMLElement* pRoot = pDoc->RootElement();
	if (!pRoot) return false;

	XMLElement* pLoginServer = pRoot->FirstChildElement("LoginServer");
	if (pLoginServer) {
		XMLElement* pTagLog = pLoginServer->FirstChildElement("Log");
		if (pTagLog) _logFile = pTagLog->Attribute("output");

		XMLElement* pTagNet = pLoginServer->FirstChildElement("Net");
		if (pTagNet) pTagNet->QueryUnsignedAttribute("port", &_netPort);
	}

	return true;
}
