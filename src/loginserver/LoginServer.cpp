#include "LoginServer.h"
#include "LoginUserManager.h"

using namespace x::login;
using namespace x::tool;

/// LoginServer
LoginServer::LoginServer(IOContext& ctx, std::string name):
	_dispatcher(std::bind(&LoginServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
	_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

	_pTcpServer = std::make_shared<TcpServer>(ctx, name);
	_pTcpServer->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
	_pTcpServer->SetConnectionCallback(std::bind(&LoginServer::OnConnection, this, std::placeholders::_1));
	_pTcpServer->SetHeartCallback(std::bind(&LoginServer::SendHeartBeat, this, std::placeholders::_1));

	_dispatcher.RegisterMessageCallback<HeartBeat>(std::bind(&LoginServer::OnHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(LoginConfig::GetInstance().GetLogFile());
	// global_logger_set_filter((expr::has_attr(tag_attr) && (tag_attr == "ProtobufCodec")));
	LoginUserManager::GetInstance().Init();
	LoginUserManager::GetInstance().RegisterMessageCallback(_dispatcher);

	_pTcpServer->Listen(LoginConfig::GetInstance().GetListenPort());
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
	auto pSend = std::make_shared<HeartBeat>();
	if (pSend) {
		unsigned int now = Now::Second();
		pSend->set_time(now);

		Buffer buf;
		ProtobufCodec::PackMessage(pSend, buf);
		pConnection->AsyncSend(buf);
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
