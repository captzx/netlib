#pragma once

#include <xcommon/Server.h>

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>
#include <xprotos/Scene.pb.h>

namespace x {
namespace gateway {

	/// class GatewayServer
	class GatewayServer : public Server, public Singleton<GatewayServer> {
	public:
		GatewayServer();
		virtual ~GatewayServer() { }

	public:
		virtual void InitModule() override;
		virtual ServerType GetServerType() override { return ServerType::GATEWAY; }

	public:

		void ForwardToLogin(unsigned int svr_id, const MessagePtr& pMsg);
		void ForwardToData(unsigned int svr_id, const MessagePtr& pMsg);
		void ForwardToScene(unsigned int svr_id, const MessagePtr& pMsg);

		void OnRequestZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RequestZoneRoleData>&);
		void OnResponseZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ResponseZoneRoleData>&);
		void OnReqCreateRole(const TcpConnectionPtr&, const std::shared_ptr<ReqCreateRole>&);
		void OnRspCreateRole(const TcpConnectionPtr&, const std::shared_ptr<RspCreateRole>&);
		void OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>&);

		void OnLoginIntoScene(const TcpConnectionPtr&, const std::shared_ptr<LoginIntoScene>&);
		
		
	};

} // namespace gateway
} // namespace x

