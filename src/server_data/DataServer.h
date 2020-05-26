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
		virtual void InitModule() override;
		virtual ServerType GetServerType() override { return ServerType::DATA; }

	public:
		void OnRequestZoneRoleData(const TcpConnectionPtr&, const std::shared_ptr<RequestZoneRoleData>&);
		void OnReqCreateRole(const TcpConnectionPtr&, const std::shared_ptr<ReqCreateRole>&);
		void OnReqEnterGame(const TcpConnectionPtr&, const std::shared_ptr<ReqEnterGame>&);
		
	private:
		unsigned int _rolecount;
	};

} // namespace data
} // namespace x

