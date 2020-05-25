#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>

namespace x {
namespace scene {

	/// class SceneServer
	class SceneServer : public Server, public Singleton<SceneServer> {
	public:
		SceneServer();
		virtual ~SceneServer() { }

	public:
		virtual void InitModule() override;
		virtual ServerType GetServerType() override { return ServerType::SCENE; }
	};

} // namespace scene
} // namespace x

