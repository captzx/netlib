#pragma once

#include <xprotos/Login.pb.h>

#include "LoginServer.h"

using x::tool::Singleton;

namespace x {

namespace login {

struct ZoneServer {
	int id = 0;
	std::string name;
	std::string ip;
	unsigned int port = 0;
	unsigned int state = 0;

	TcpConnectionWeakPtr wpConnection;
};

class ZoneServerManager : public Singleton<ZoneServerManager> {
public:
	ZoneServerManager() {}

public:
	bool InitServerList();
	void RegisterMessageCallback();


public:
	bool SendAllZoneList(const TcpConnectionPtr& pConnection);
	void OnSelectZoneServer(const TcpConnectionPtr&, const std::shared_ptr<SelectZoneServer>&);
	void OnResponseZoneRoleData(const TcpConnectionPtr& pConnection, const std::shared_ptr<ResponseZoneRoleData>& pRecv);
	void OnReqCreateRole(const TcpConnectionPtr& pConnection, const std::shared_ptr<ReqCreateRole>& pRecv);
	void OnRspCreateRole(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspCreateRole>& pRecv);
	void OnReqEnterGame(const TcpConnectionPtr& pConnection, const std::shared_ptr<ReqEnterGame>& pRecv);
	void OnRspEnterGame(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspEnterGame>& pRecv);
	
private:
	std::map<int, ZoneServer> _zoneList;
};

}

}