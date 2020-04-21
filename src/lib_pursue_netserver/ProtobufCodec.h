#pragma once

#include "Singleton.h"
#include "TcpConnection.h"
#include <boost/asio.hpp>
#include <google/protobuf/message.h>

#include "Buffer.h"

#include <memory>

using piece::tool::Singleton;
using piece::net::Buffer;

namespace piece {

namespace net {

const static int header_len = sizeof(int32_t);
const static int message_min_len = 2 * header_len + 2; // nameLen + checkSum + message name(at least one char + '\n')
const static int message_max_len = 64 * 1024 * 1024;

using MessagePtr = std::shared_ptr<google::protobuf::Message> ;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
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
