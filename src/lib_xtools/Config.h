#pragma once

#include <string>
#include <tinyxml2.h>

using namespace tinyxml2;

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

struct ServerCfg {
	std::string IP;
	unsigned int Port;
	unsigned int MaxConn;
	std::string LogFile;
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

		XMLElement* pLoginServer = pRoot->FirstChildElement("LoginServer");
		if (pLoginServer) {
			XMLElement* pTagLog = pLoginServer->FirstChildElement("Log");
			if (pTagLog) _logFile = pTagLog->Attribute("output");

			XMLElement* pTagNet = pLoginServer->FirstChildElement("Net");
			if (pTagNet) pTagNet->QueryUnsignedAttribute("port", &_netPort);
		}

		return true;
	}

public:
	unsigned int GetServerCfgByType() { return _netPort; }

private:
	std::map<int, ServerCfg> _serverCfgs;
};

} // namespace tool

} // namespace x

