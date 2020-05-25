#include "DataServer.h"

using namespace x::data;

/// DataServer
DataServer::DataServer(){

}

void DataServer::InitModule() {
	ProtobufDispatcher& dispatcher = DataServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestPlayerLoginData>(std::bind(&DataServer::OnRequestPlayerLoginData, this, std::placeholders::_1, std::placeholders::_2));

}

void DataServer::OnRequestPlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<RequestPlayerLoginData>& pRecv) {
	log(warning, "DataServer") << "async send RequestPlayerLoginData, act_id = " << pRecv->act_id();
}