#pragma once

#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>

using namespace x;
using namespace x::tool;
using namespace x::net;

namespace x {

class Server {
public:
	Server();
	virtual ~Server() {}

public:
	virtual void Start(uint32_t id) final;

	virtual ProtobufDispatcher& GetDispatcher() final { return _dispatcher; }
	virtual ProtobufCodec& GetCodec() final { return _codec; }
	virtual TcpServicePtr& GetTcpService() final { return _pTcpService; }
	virtual DBServicePtr& GetDBService() final { return _pDBService; }

	virtual void InitModule() = 0;
	virtual ServerType GetServerType() = 0;

public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);

public:
	void OnAtvConnection(const TcpConnectionPtr&);
	void OnPsvConnection(const TcpConnectionPtr&);
	virtual void OnActiveHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<ActiveHeartBeat>&);
	void OnPassiveHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<PassiveHeartBeat>&);
	void OnActiveConnection(const TcpConnectionPtr& pConnection, const std::shared_ptr<ActiveConnection>& pMsg);
public:
	void CheckHeartBeat();

public:
	uint32_t GetStartTime() { return _startTime; }

public:
	static ServerType TYPE;

protected:
	uint32_t _startTime;
	ServerCfg* _pSvrCfg;

	TcpServicePtr _pTcpService;
	DBServicePtr _pDBService;

	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;

	uint32_t _heartbeatPeriod;

	std::map<std::pair<uint32_t, uint32_t>, TcpConnectionWeakPtr> _connections; // { type, id } -> connection
};

} // namespace x

