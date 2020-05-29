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

enum class ServerType : uint32_t{
	UNDEFINED = 0,
	SUPERVISOR = 1,
	GATEWAY = 2,
	LOGIN = 3,
	DATA = 4,
	CENTER = 5,
	SCENE = 6
};

struct ServerCfg {
	uint32_t Type = 0;
	std::string Name;
	uint32_t ID = 0;
	std::string IP;
	uint32_t Port = 0;
	std::string LogFile;
	uint32_t LogLevel = 0;

	struct ConnectDB {
		uint32_t Type;
		std::string Url;
	};
	std::vector<ConnectDB> ConnectDBCfgs;

	struct ConnectServer {
		uint32_t Type = 0;
		uint32_t ID = 0;
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
				serverCfg.ID = pServer->UnsignedAttribute("ID");
				serverCfg.Name = std::string(pServer->Attribute("Name"));
				serverCfg.IP = std::string(pServer->Attribute("IP"));
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
						cServer.ID = pPerSvr->UnsignedAttribute("ID");

						serverCfg.ConnectSvrCfgs.push_back(cServer);
					}
				}

				_serverCfgs[{serverCfg.Type, serverCfg.ID}] = serverCfg;
			}
		}

		return true;
	}

public:
	ServerCfg* GetServerCfgByType(const std::pair<uint32_t, uint32_t>& pair) {
		auto it = _serverCfgs.find(pair);
		if (it != _serverCfgs.end()) {
			return &(it->second);
		}

		return nullptr;
	}

private:

	std::map<std::pair<uint32_t, uint32_t>, ServerCfg> _serverCfgs; // {type, id} => ServerCfg
};

void filter_blank(std::string& str) {
	std::regex reg("[[:s:]]");
	str = std::regex_replace(str, reg, "");
}

void token_split(const std::string& str, std::vector<std::string>& result, const char* regex = "\\[[[:d:]],[[:d:]]\\]") {
	std::regex reg(regex);
	sregex_token_iterator pos(str.cbegin(), str.cend(), reg, 0), end;
	for (; pos != end; ++pos) result.push_back(*pos);
}

void to_pairs(std::string& str, std::vector<std::pair<int, int>>& pairs, const char* regex = "([[:d:]]+)[[:punct:]]([[:d:]]+)") {
	filter_blank(str);

	std::vector<std::string> strs;
	token_split(str, strs);

	std::regex reg(regex);
	for (auto str : strs) {
		sregex_iterator pos(str.cbegin(), str.cend(), reg), end;
		for (; pos != end; ++pos) pairs.push_back({ std::atoi(pos->str(1).c_str()), std::atoi(pos->str(2).c_str()) });
	}
}
} // namespace tool

} // namespace x

