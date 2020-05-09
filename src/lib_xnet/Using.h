#pragma once

#include <xtools/Common.h>
#include <xtools/Logger.h>

#include <boost/asio.hpp>
using boost::asio::ip::tcp;
using SocketPtr = std::shared_ptr<tcp::socket>;

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
using MessagePtr = std::shared_ptr<google::protobuf::Message>;

namespace x {

namespace net {

class Buffer;
class TcpConnection;

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer&)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;

using ProtobufMessageCallback = std::function<void(const TcpConnectionPtr&, const MessagePtr&)>;
using CallbackMap = std::map<const google::protobuf::Descriptor*, ProtobufMessageCallback>;

}

}