#pragma once

#include <memory>

#include <xtools/Singleton.h>
#include <boost/asio.hpp>
#include <google/protobuf/message.h>

#include "Buffer.h"
#include "TcpConnection.h"

using x::tool::Singleton;
using x::net::Buffer;

namespace x {

namespace net {

using MessagePtr = std::shared_ptr<google::protobuf::Message> ;
class ProtobufCodec : public Singleton<ProtobufCodec> {
public:
	static void PackMessage(Buffer&, const google::protobuf::Message&);
	static MessagePtr Parse(const char* buf, int len);
	static google::protobuf::Message* CreateMessage(const std::string& typeName);
	
	void Recv(const TcpConnectionPtr& conn, Buffer* buf);
	void Send(const TcpConnectionPtr& conn, const google::protobuf::Message& message);
};

}

}
