#include "LoginServer.h"

#include <xprotos/login.pb.h>

using namespace x::login;
using namespace x::tool;

/// LoginServer
LoginServer::LoginServer(std::string name):
	_dispatcher(std::bind(&LoginServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::Recv, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {
	_dispatcher.RegisterMessageCallback(SearchRequest::descriptor(), std::bind(&LoginServer::onSearchRequest, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback(SearchResponse::descriptor(), std::bind(&LoginServer::onSearchResponse, this, std::placeholders::_1, std::placeholders::_2));

	_pTcpServer = std::make_shared<NetServer>(name);
	_pTcpServer->SetMessageCallback(std::bind(&ProtobufCodec::Recv, &_codec, std::placeholders::_1, std::placeholders::_2));
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(LoginConfig::GetInstance().GetLogFile());
	_pTcpServer->Listen(LoginConfig::GetInstance().GetListenPort());
}

void LoginServer::	DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug) << "login server default message call back.";
}

void LoginServer::onSearchRequest(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug) << "on SearchRequest.";
}

void LoginServer::onSearchResponse(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug) << "on SearchResponse.";
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
