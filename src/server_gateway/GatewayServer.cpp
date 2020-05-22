#include "GatewayServer.h"

using namespace x::gateway;
using namespace x::tool;

/// GatewayServer
GatewayServer::GatewayServer(std::string name):
	_dispatcher(std::bind(&GatewayServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

	_pTcpService = std::make_shared<TcpService>(name);

	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&GatewayServer::OnConnection, this, std::placeholders::_1));
}

void GatewayServer::Start() {
	GatewayConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(GatewayConfig::GetInstance().GetLogFile());
	global_logger_set_filter(severity >= trivial);
	_pTcpService->AsyncListen(GatewayConfig::GetInstance().GetListenPort());
	_pTcpService->Start();
	log(debug, "GatewayServer") << "gateway server start.";
}

void GatewayServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "GatewayServer") << "login server default message call back.";
}

void GatewayServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "GatewayServer") << "on Connection.";
}

/// GatewayConfig
bool GatewayConfig::Parse() {
	const std::shared_ptr<XMLDocument>& pDoc = GetXmlDoc();
	if (!pDoc) {
		std::cout << "xml document not found, is file exist?" << std::endl;
		return false;
	}

	XMLElement* pRoot = pDoc->RootElement();
	if (!pRoot) return false;

	XMLElement* pGatewayServer = pRoot->FirstChildElement("GatewayServer");
	if (pGatewayServer) {
		XMLElement* pTagLog = pGatewayServer->FirstChildElement("Log");
		if (pTagLog) _logFile = pTagLog->Attribute("output");

		XMLElement* pTagNet = pGatewayServer->FirstChildElement("Net");
		if (pTagNet) pTagNet->QueryUnsignedAttribute("port", &_netPort);
	}

	return true;
}
