#pragma once

#include "Using.h"

namespace x {

namespace net {
/// ProtobufCodec
class ProtobufCodec {
public:
	ProtobufCodec(const ProtobufMessageCallback& callback) {
		_messageCallback = callback;
	}
public:
	void PackMessage(const MessagePtr&, Buffer&);
	MessagePtr ParseDataPackage(const char*, int);
	void RecvMessage(const TcpConnectionPtr&, Buffer&);
	void SendMessage(const TcpConnectionPtr&, const MessagePtr&);
public:
	static MessagePtr CreateMessage(const std::string&);

private:
	ProtobufMessageCallback _messageCallback;
};

/// ProtobufDispatcher
class ProtobufDispatcher
{
public:
	explicit ProtobufDispatcher(const ProtobufMessageCallback& callback){
		_defaultCallback
	}

public:
	void RecvMessage(const TcpConnectionPtr&, const MessagePtr& ) const;
	void RegisterMessageCallback(const Descriptor*, const MessageDispatcherCallback&);

private:
	CallbackMap _callbacks;
	ProtobufMessageCallback _defaultCallback;
};

}

}
