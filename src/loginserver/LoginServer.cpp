#include "LoginServer.h"

#include <xprotos/login.pb.h>

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

	_dispatcher.RegisterMessageCallback(SearchRequest::descriptor(), std::bind(&LoginServer::OnSearchRequest, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback(SearchResponse::descriptor(), std::bind(&LoginServer::OnSearchResponse, this, std::placeholders::_1, std::placeholders::_2));
	_dispatcher.RegisterMessageCallback(HeartBeat::descriptor(), std::bind(&LoginServer::OnHertBeat, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginServer::Start() {
	LoginConfig::GetInstance().LoadFile("config.xml");
	global_logger_init(LoginConfig::GetInstance().GetLogFile());
	_pTcpServer->Listen(LoginConfig::GetInstance().GetListenPort());
}

void LoginServer::DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug) << "login server default message call back.";
}

void LoginServer::OnSearchRequest(const TcpConnectionPtr& pConnection, const MessagePtr& pMessage) {
	log(debug) << "on SearchRequest. connection strong ref: " << pConnection.use_count();

	auto pMsg = std::dynamic_pointer_cast<SearchRequest>(pMessage);
	log(debug) << "SearchRequest query: ." << pMsg->query();
	log(debug) << "SearchRequest page_number: " << pMsg->page_number();
	log(debug) << "SearchRequest result_per_page: " << pMsg->result_per_page();

	{
		auto pMsg = std::make_shared<SearchRequest>();
		if (pMsg) {
			pMsg->set_query("captzx");
			pMsg->set_page_number(1);
			pMsg->set_result_per_page(1);
			Buffer buf;
			ProtobufCodec::PackMessage(pMsg, buf);
			pConnection->AsyncSend(buf);
			std::cout << "Enter message: len = " << buf.Readable() << std::endl;
		}
	}
}

void LoginServer::OnHertBeat(const TcpConnectionPtr& pConnection, const MessagePtr& pMessage) {
	auto pMsg = std::dynamic_pointer_cast<HeartBeat>(pMessage);
	pConnection->SetLastHeartBeatTime(pMsg->time());
}

void LoginServer::SendHeartBeat(const TcpConnectionPtr& pConnection) {
	auto pSend = std::make_shared<HeartBeat>();
	if (pSend) {
		unsigned int now = GetSystemTime();
		pSend->set_time(now);

		Buffer buf;
		ProtobufCodec::PackMessage(pSend, buf);
		pConnection->AsyncSend(buf);
	}
}

void LoginServer::OnSearchResponse(const TcpConnectionPtr&, const MessagePtr&) {
	log(debug) << "on SearchResponse.";
}

void LoginServer::OnConnection(const TcpConnectionPtr&) {
	log(debug) << "on Connection.";
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
