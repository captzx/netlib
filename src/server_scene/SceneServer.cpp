#include "SceneServer.h"

using namespace x::scene;

/// SceneServer
SceneServer::SceneServer(){

}

void SceneServer::InitModule() {
	ProtobufDispatcher& dispatcher = SceneServer::GetInstance().GetDispatcher();
	dispatcher.RegisterMessageCallback<LoginIntoScene>(std::bind(&SceneServer::OnLoginIntoScene, this, std::placeholders::_1, std::placeholders::_2));
	
}

void SceneServer::OnLoginIntoScene(const TcpConnectionPtr& pConn, const std::shared_ptr<LoginIntoScene>& pRecv) {
	log(debug, "SceneServer") << "recv LoginIntoScene, actid = " << pRecv->act_id() << "roleid = " << pRecv->role_id() << ", name = " << pRecv->name();
}


int main(int argc, char* argv[]) {
	if (argc != 2)  std::cout << "Usage: " << argv[0] << " id\n";

	GlobalConfig::GetInstance().LoadFile("config.xml");
	SceneServer::GetInstance().Start(atoi(argv[1]));

	return 0;
}