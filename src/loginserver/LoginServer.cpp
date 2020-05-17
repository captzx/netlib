#include "LoginServer.h"

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
	_dispatcher.RegisterMessageCallback<RequestRegister>(std::bind(&LoginServer::OnRequestRegister, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback<RequestRsaPublicKey>(std::bind(&LoginServer::OnRequestRsaPublicKey, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");

	x::tool::GenerateRSAKey(_private_key, _public_key);
	
	global_logger_init(LoginConfig::GetInstance().GetLogFile());

	// global_logger_set_filter((expr::has_attr(tag_attr) && (tag_attr == "ProtobufCodec")));

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

void LoginServer::OnRequestRegister(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRegister>& pMsg) {
	std::string account = pMsg->account();
	std::string password = RSADecrypt(_private_key, pMsg->password());

	log(debug, "LoginServer") << "register account: "<< account << ", password: " << password;

	if (VerifyAccount(account) && SaveAccount(account, password)) SendRegisterResult(pConnection, 1);
}
void LoginServer::OnRequestRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRsaPublicKey>&) {
	auto pMsg = std::make_shared<ResponseRsaPublicKey>();
	if (pMsg) {
		pMsg->set_publickey(_public_key);

		Buffer buf;
		ProtobufCodec::PackMessage(pMsg, buf);
		pConnection->AsyncSend(buf);
	}
}

bool LoginServer::SendHeartBeat(const TcpConnectionPtr& pConnection) {
	auto pSend = std::make_shared<HeartBeat>();
	if (pSend) {
		unsigned int now = GetSystemTime();
		pSend->set_time(now);

		Buffer buf;
		ProtobufCodec::PackMessage(pSend, buf);
		pConnection->AsyncSend(buf);
	}

	return true;
}
bool LoginServer::SendRegisterResult(const TcpConnectionPtr& pConnection, int result) {
	auto pMsg = std::make_shared<ResponseRegister>();
	if (pMsg) {
		pMsg->set_result(result);

		Buffer buf;
		ProtobufCodec::PackMessage(pMsg, buf);
		pConnection->AsyncSend(buf);
	}

	return true;
}

bool LoginServer::VerifyPassword(const std::string& account, const std::string& password) {
	log(debug, "LoginServer") << "todo: verify password";
	return true;
}

bool LoginServer::VerifyAccount(const std::string& account) {
	log(debug, "LoginServer") << "todo: verify account";
	return true;
}

bool LoginServer::SaveAccount(const std::string& account, const std::string& password) {
	std::string result = PBKDFEncrypt(password, SHA3_256Hash(password)); // fix, don't use password generate salt
	
	log(debug, "LoginServer") << "PBKDFEncrypt result:" << result;
	log(debug, "LoginServer") << "todo: save account to db";
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
