#pragma once

#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>

using namespace x;
using namespace x::tool;
using namespace x::net;

namespace x {

/// class SupervisorServer
	class Server {
	public:
		Server();
		virtual ~Server() {}

	public:
		virtual void Start() final;

		virtual ProtobufDispatcher& GetDispatcher() final { return _dispatcher; }
		virtual ProtobufCodec& GetCodec() final { return _codec; }
		virtual TcpServicePtr& GetTcpService() final { return _pTcpService; }
		virtual DBServicePtr& GetDBService() final { return _pDBService; }

		virtual void InitModule() = 0;
		virtual ServerType GetServerType() = 0;

public:
	void InitConfig();
	void InitLog();
	void InitDBService();
	void InitTcpService();

public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&);
	void SetStartCallBack();

public:
	void OnConnection(const TcpConnectionPtr&);
	virtual void OnActiveHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<ActiveHeartBeat>&);
	void OnPassiveHeartBeat(const TcpConnectionPtr&, const std::shared_ptr<PassiveHeartBeat>&);

public:
	bool SendStateToActiveConnection();
	void CheckHeartBeat();

public:
	unsigned int GetStartTime() { return _startTime; }

public:
	static ServerType TYPE;

protected:
	unsigned int _startTime;
	ServerCfg* _pSvrCfg;

	TcpServicePtr _pTcpService;
	DBServicePtr _pDBService;

	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;

	unsigned int _heartbeatPeriod;
};

} // namespace x

