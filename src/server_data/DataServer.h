#pragma once

#include <xcommon/Server.h>

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>
#include <xprotos/Scene.pb.h>

namespace x {
namespace data {

	/// class DataServer
	class DataServer : public Server, public Singleton<DataServer> {
	public:
		DataServer();
		virtual ~DataServer() { }

	public:
		virtual ServerType GetServerType() override { return ServerType::DATA; }
		virtual void InitModule() override;

	public:
		void OnReqZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<ReqZoneRoleData>&);
		void OnReqCreateRole(const TcpConnectionPtr&, const std::shared_ptr<ReqCreateRole>&);
		void OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>&);
		
	private:
		uint32_t _rolecount;
	};

} // namespace data
} // namespace x

