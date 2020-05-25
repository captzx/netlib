#pragma once

#include "Server.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

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
		void OnRequestPlayerLoginData(const TcpConnectionPtr&, const std::shared_ptr<RequestPlayerLoginData>&);
	};

} // namespace data
} // namespace x

