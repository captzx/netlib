#include "LoginServer.h"
#include "LoginUserManager.h"

using namespace x::login;
using namespace x::tool;

DBConnectionPtr LoginServer::_pDBLoginConnection = std::make_shared<DBConnection>("mysqlx://zx:Luck25.zx@localhost");

/// LoginServer
LoginServer::LoginServer(std::string name):
	_dispatcher(std::bind(&LoginServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

	_pTcpService = std::make_shared<TcpService>(name);

	_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpService->SetConnectionCallback(std::bind(&LoginServer::OnConnection, this, std::placeholders::_1));
	// _pTcpService->SetHeartCallback(std::bind(&LoginServer::SendHeartBeat, this, std::placeholders::_1));

	_dispatcher.RegisterMessageCallback<HeartBeat>(std::bind(&LoginServer::OnHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(LoginConfig::GetInstance().GetLogFile());
	global_logger_set_filter(severity >= trivial);
	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback(_dispatcher);

	_pGateWayConnection = _pTcpService->AsyncConnect("127.0.0.1", 1235);

	_pTcpService->AsyncListen(LoginConfig::GetInstance().GetListenPort());
	_pTcpService->Start();

	log(debug, "LoginServer") << "login server start.";
}

void LoginServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug, "LoginServer") << "login server default message call back.";
}

void LoginServer::OnConnection(const TcpConnectionPtr&) {
	log(debug, "LoginServer") << "on Connection.";
}

void LoginServer::OnHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<HeartBeat>& pMessage) {
	pConnection->SetLastHeartBeatTime(pMessage->time());
}

bool LoginServer::SendHeartBeat(const TcpConnectionPtr& pConnection) {
	auto pMsg = std::make_shared<HeartBeat>();
	if (pMsg) {
		unsigned int now = Now::Second();
		pMsg->set_time(now);
		pConnection->AsyncSend(pMsg);
	}

	return true;
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
