#include "common.h"

#include "LoginServer.h"

#include <tinyxml2.h>

using namespace piece::net;
using namespace piece::tool;

void LoginServer::Start() {
	_LoadConfig("config.xml");

	_Init();

	_tcpServer.Listen(_lsCfg.port);

}

bool LoginServer::ReloadConfig() {

	return true;
}

bool LoginServer::_Init() {
	global_logger_init(_lsCfg.file);

	_tcpServer.Init();

	return true;
}

bool LoginServer::_LoadConfig(std::string file) {
	if (file.empty()) {
		std::cerr << "config file name is empty!" << std::endl;
		return false;
	}

	using namespace tinyxml2;

	XMLDocument doc;
	XMLError ret = doc.LoadFile(file.c_str());
	if (ret != 0) {
		std::cerr << "fail to load xml file: " << file << std::endl;
		return false;
	}

	XMLElement* pRoot = doc.RootElement();
	if (!pRoot) return false;

	XMLElement* pLoginServer = pRoot->FirstChildElement("LoginServer");
	if (pLoginServer) {
		XMLElement* pLog = pLoginServer->FirstChildElement("Log");
		if (pLog) {
			_lsCfg.file = pLog->Attribute("file");
		}

		XMLElement* pNet = pLoginServer->FirstChildElement("Net");
		if (pNet) {
			pNet->QueryUnsignedAttribute("port", &_lsCfg.port);
		}
	}

	return true;
}
