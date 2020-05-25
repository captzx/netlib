#include "DataServer.h"

using namespace x::data;

/// DataServer
DataServer::DataServer(){

}

void DataServer::InitModule() {
	ProtobufDispatcher& dispatcher = DataServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<RequestPlayerLoginData>(std::bind(&DataServer::OnRequestPlayerLoginData, this, std::placeholders::_1, std::placeholders::_2));

}

void DataServer::OnRequestPlayerLoginData(const TcpConnectionPtr& pConn, const std::shared_ptr<RequestPlayerLoginData>& pRecv) {
	log(warning, "DataServer") << "async send RequestPlayerLoginData, act_id = " << pRecv->act_id();

	std::ostringstream oss;

	oss << "SELECT * FROM x_data.role WHERE role.`actid` = '" << pRecv->act_id() << "';";

	DBServicePtr& pDBService = DataServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_data)->ExecuteSql(oss.str());
	
	auto pSend = std::make_shared<ResponsePlayerLoginData>();
	pSend->set_svr_id(pRecv->svr_id()); // 1
	pSend->set_act_id(pRecv->act_id());
	if (result.count() == 0) {
		log(debug, "DataServer") << "player data == null, new player please!";
	}
	else pSend->set_data(1); // test
	pConn->AsyncSend(pSend);
}