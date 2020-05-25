#include "SceneServer.h"

using x::scene::SceneServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	SceneServer::GetInstance().Start();

	return 0;
}