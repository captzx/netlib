#pragma once

#include <xcommon/Server.h>

#include <xprotos/Server.pb.h>

namespace x {
namespace center {

	/// class CenterServer
	class CenterServer : public Server, public Singleton<CenterServer> {
	public:
		CenterServer();
		virtual ~CenterServer() { }

	public:
		virtual ServerType GetServerType() override { return ServerType::CENTER; }
		virtual void InitModule() override;
	};

} // namespace center
} // namespace x

