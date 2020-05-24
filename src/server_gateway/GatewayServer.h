#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>

using namespace x::tool;
using namespace x::net; 

namespace x {
namespace gateway {

	/// class GatewayServer
	class GatewayServer : public Server, public Singleton<GatewayServer> {
	public:
		GatewayServer();
		virtual ~GatewayServer() { }

	public:
		virtual void InitModule() override;
		virtual ServerType GetServerType() override { return TYPE; }

	public:
		const static ServerType TYPE = ServerType::GATEWAY;
	};

} // namespace gateway
} // namespace x

