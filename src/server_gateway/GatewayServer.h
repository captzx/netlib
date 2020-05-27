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
		virtual ServerType GetServerType() override { return ServerType::GATEWAY; }
		virtual void InitModule() override;

	public:

		void ForwardToLogin(uint32_t svr_id, const MessagePtr&);
		void ForwardToData(uint32_t svr_id, const MessagePtr&);
		void ForwardToScene(uint32_t svr_id, const MessagePtr&);

		void OnReqZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ReqZoneRoleData>&);
		void OnRspZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RspZoneRoleData>&);
		void OnReqCreateRole(const TcpConnectionPtr&, const std::shared_ptr<ReqCreateRole>&);
		void OnRspCreateRole(const TcpConnectionPtr&, const std::shared_ptr<RspCreateRole>&);
		void OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>&);

		void OnLoginIntoScene(const TcpConnectionPtr&, const std::shared_ptr<LoginIntoScene>&);
		
		
	};

} // namespace gateway
} // namespace x

