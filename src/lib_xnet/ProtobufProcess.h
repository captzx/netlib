#pragma once

#include "Using.h"
#include "Buffer.h"
#include "TcpService.h"
#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
using google::protobuf::Message;
using google::protobuf::Descriptor;

namespace x {

namespace net {

using MessagePtr = std::shared_ptr<Message>;
using ProtobufMessageCallback = std::function<void(const TcpConnectionPtr&, const MessagePtr&)>;
using CallbackMap = std::map<const Descriptor*, ProtobufMessageCallback>;

/// ProtobufCodec
class ProtobufCodec {
public:
	ProtobufCodec(const ProtobufMessageCallback& callback) {
		_messageCallback = callback;
	}
public:
	static void PackMessage(const MessagePtr&, Buffer&);
	static MessagePtr CreateMessageByName(const std::string&);
	static MessagePtr ParseDataPackage(const char*, int);
	void RecvMessage(const TcpConnectionPtr&, Buffer&);
	void SendMessage(const TcpConnectionPtr&, const MessagePtr&);

private:
	ProtobufMessageCallback _messageCallback;
};

/// ProtobufDispatcher
class ProtobufDispatcher
{
public:
	explicit ProtobufDispatcher(const ProtobufMessageCallback& callback){
		_defaultCallback = callback;
	}

public:
	void RecvMessage(const TcpConnectionPtr&, const MessagePtr& ) const;
	void RegisterMessageCallback(const Descriptor*, const ProtobufMessageCallback&);

private:
	CallbackMap _callbacks;
	ProtobufMessageCallback _defaultCallback;
};

}

}
