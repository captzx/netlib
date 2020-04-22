#include "common.h"
#include "LoginConfig.h"

using namespace x::login;

bool LoginConfig::Parse() {
	const std::shared_ptr<XMLDocument>& pDoc = GetXmlDoc();
	if (!pDoc) {
		std::cout << "xml document not found, is file exist?" << std::endl;
		return false;
	}

	XMLElement* pRoot = pDoc->RootElement();
	if (!pRoot) return false;

	XMLElement* pTagLS = pRoot->FirstChildElement("LoginServer");
	if (pTagLS) {
		XMLElement* pTagLog = pTagLS->FirstChildElement("Log");
		if (pTagLog) _logfile = pTagLog->Attribute("file");

		XMLElement* pTagNet = pTagLS->FirstChildElement("Net");
		if (pTagNet) pTagNet->QueryUnsignedAttribute("port", &_port);
	}

	return true;
}