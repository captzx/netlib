#pragma once

#include <xtools/Singleton.h>
#include "Using.h"
#include "Buffer.h"
#include "TcpConnection.h"

using x::tool::Singleton;
using x::net::Buffer;

namespace x {

namespace net {

class ProtobufCodec : public Singleton<ProtobufCodec> {
public:
	ProtobufCodec(ProtobufMessageCallback callback) :
		_messageCallback(callback) {
	}
public:
	static void PackMessage(Buffer&, const google::protobuf::Message&);
	static MessagePtr Parse(const char* buf, int len);
	static google::protobuf::Message* CreateMessage(const std::string& typeName);
	
	void Recv(const TcpConnectionPtr& conn, Buffer&);
	void Send(const TcpConnectionPtr& conn, const google::protobuf::Message& message);

private:
	ProtobufMessageCallback _messageCallback;
};

}

}
