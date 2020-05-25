#include "DataServer.h"

using x::data::DataServer;

int main() {
	GlobalConfig::GetInstance().LoadFile("config.xml");
	DataServer::GetInstance().Start();

	return 0;
}