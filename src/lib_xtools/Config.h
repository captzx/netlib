#pragma once

#include <string>

#include <tinyxml2.h>
using namespace tinyxml2;

#include "Singleton.h"

namespace x {

namespace tool {

class Config {
public:
	virtual bool Parse() = 0;

public:
	virtual bool LoadFile(std::string file) final {
		if (file.empty()) return false;

		_pXmlDoc = std::make_shared<XMLDocument>();

		auto result = _pXmlDoc->LoadFile(file.c_str());
		if (result) return false;

		_fileName = file;

		return Parse();
	}

	virtual const std::string& GetFileName() final { return _fileName; }
	virtual const std::shared_ptr<XMLDocument>& GetXmlDoc() final { return _pXmlDoc; }

private:
	std::shared_ptr<XMLDocument> _pXmlDoc;
	std::string _fileName;
};

enum class ServerType : unsigned int{
	UNDEFINED = 0,
	SUPERVISOR = 1,
	GATEWAY = 2,
	LOGIN = 3,
	DATA = 4,
	CENTER = 5,
	SCENE = 6
};

struct ServerCfg {
	unsigned int Type = 0;
	unsigned int Port = 0;
	unsigned int LogLevel = 0;
	std::string Name;
	std::string LogFile;

	struct ConnectDB {
		unsigned int Type;
		std::string Url;
	};
	std::vector<ConnectDB> ConnectDBCfgs;

	struct ConnectServer {
		unsigned int Type = 0;
		unsigned int Port = 0;
		std::string IP;
	};
	std::vector<ConnectServer> ConnectSvrCfgs;
};

/// class GlobalConfig
class GlobalConfig : public Config, public Singleton<GlobalConfig> {
public:
	virtual bool Parse() override {
		const std::shared_ptr<XMLDocument>& pDoc = GetXmlDoc();
		if (!pDoc) {
			std::cout << "xml document not found, is file exist?" << std::endl;
			return false;
		}

		XMLElement* pRoot = pDoc->RootElement();
		if (!pRoot) return false;

		XMLElement* pServerInfo = pRoot->FirstChildElement("ServerInfo");
		if (pServerInfo) {
			XMLElement* pServer = pServerInfo->FirstChildElement("_A_Server");
			for (; pServer ; pServer = pServer->NextSiblingElement("_A_Server")) {
				ServerCfg serverCfg;
				serverCfg.Type = pServer->UnsignedAttribute("Type");
				serverCfg.Name = std::string(pServer->Attribute("Name"));
				serverCfg.Port = pServer->UnsignedAttribute("Port");
				serverCfg.LogFile = std::string(pServer->Attribute("LogFile"));
				serverCfg.LogLevel = pServer->UnsignedAttribute("LogLevel");

				XMLElement* pConnectDB = pServer->FirstChildElement("ConnectDB");
				if (pConnectDB) {
					XMLElement* pPerDB = pConnectDB->FirstChildElement("_A_Connect");
					for (; pPerDB; pPerDB = pPerDB->NextSiblingElement("_A_Connect")) {
						ServerCfg::ConnectDB cDB;
						cDB.Type = pPerDB->UnsignedAttribute("Type");
						cDB.Url = std::string(pPerDB->Attribute("Url"));

						serverCfg.ConnectDBCfgs.push_back(cDB);
					}
				}
				

				XMLElement* pConnectServer = pServer->FirstChildElement("ConnectServer");
				if (pConnectServer) {
					XMLElement* pPerSvr = pConnectServer->FirstChildElement("_A_Connect");
					for (; pPerSvr; pPerSvr = pPerSvr->NextSiblingElement("_A_Connect")) {
						ServerCfg::ConnectServer cServer;
						cServer.Type = pPerSvr->UnsignedAttribute("Type");
						cServer.IP = std::string(pPerSvr->Attribute("IP"));
						cServer.Port = pPerSvr->UnsignedAttribute("Port");

						serverCfg.ConnectSvrCfgs.push_back(cServer);
					}
				}

				_serverCfgs[serverCfg.Type] = serverCfg;
			}
		}

		return true;
	}

public:
	ServerCfg* GetServerCfgByType(ServerType type) {
		auto it = _serverCfgs.find((int)type);
		if (it != _serverCfgs.end()) {
			return &(it->second);
		}

		return nullptr;
	}

private:
	std::map<int, ServerCfg> _serverCfgs;
};

} // namespace tool

} // namespace x

