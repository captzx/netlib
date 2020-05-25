#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>

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
	};

} // namespace gateway
} // namespace x

