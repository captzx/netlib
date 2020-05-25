#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

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
		void OnRequestPlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<RequestPlayerLoginData>&);
		void OnResponsePlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<ResponsePlayerLoginData>&);
	};

} // namespace gateway
} // namespace x

