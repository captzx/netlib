#pragma once

#include <xcommon/Server.h>
#include <xprotos/Server.pb.h>
#include <xprotos/Scene.pb.h>

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

	public:
		void OnLoginIntoScene(const TcpConnectionPtr&, const std::shared_ptr<LoginIntoScene>&);

	};

} // namespace scene
} // namespace x

