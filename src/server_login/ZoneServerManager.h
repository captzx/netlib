#pragma once

#include <xprotos/Login.pb.h>

#include "LoginServer.h"

namespace x {

namespace login {

struct ZoneServer {
	int32_t id = 0;
	std::string name;
	std::string ip;
	uint32_t port = 0;
	uint32_t state = 0;

	TcpConnectionWeakPtr wpConnection;
};

class ZoneServerManager : public Singleton<ZoneServerManager> {
public:
	ZoneServerManager() {}

public:
	bool InitServerList();
	void RegisterMessageCallback();


public:
	bool SendAllZoneList(const TcpConnectionPtr&);
	void OnSelectZoneServer(const TcpConnectionPtr&, const std::shared_ptr<SelectZoneServer>&);
	void OnRspZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RspZoneRoleData>&);
	void OnReqCreateRole(const TcpConnectionPtr& , const std::shared_ptr<ReqCreateRole>&);
	void OnRspCreateRole(const TcpConnectionPtr&, const std::shared_ptr<RspCreateRole>&);
	void OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>&);
	void OnRspEnterGame(const TcpConnectionPtr&, const std::shared_ptr<RspEnterGame>&);
	
private:
	std::map<int32_t, ZoneServer> _zoneList;
};

}

}