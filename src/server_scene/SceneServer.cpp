#include "SceneServer.h"

using namespace x::scene;

/// SceneServer
SceneServer::SceneServer(){

}

void SceneServer::InitModule() {
	GET_MESSAGE_DISPATCHER(SceneServer::GetInstance().GetDispatcher());
	REGISTER_MESSAGE_CALL_BACK(SceneServer, this, LoginIntoScene);
	
}

void SceneServer::OnLoginIntoScene(const TcpConnectionPtr& pConn, const std::shared_ptr<LoginIntoScene>& pRecv) {
	log(debug, "SceneServer") << "recv LoginIntoScene, actid = " << pRecv->act_id() << "roleid = " << pRecv->role_id() << ", name = " << pRecv->name();
}


int32_t main(int32_t argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	SceneServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}