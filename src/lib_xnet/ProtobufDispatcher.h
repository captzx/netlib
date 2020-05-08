#pragma once

#include "Using.h"
#include "Buffer.h"
#include "TcpConnection.h"

namespace x {

namespace net {

class ProtobufDispatcher
{
public:
	explicit ProtobufDispatcher(const ProtobufMessageCallback& callback)
		: _defaultCallback(callback)
	{
	}

	void Recv(const TcpConnectionPtr& conn, const MessagePtr& message) const
	{
		auto it = _callbacks.find(message->GetDescriptor());
		if (it != _callbacks.end()) it->second(conn, message);
		else _defaultCallback(conn, message);
	}

	void RegisterMessageCallback(const google::protobuf::Descriptor* desc, const ProtobufMessageCallback& callback)
	{
		_callbacks[desc] = callback;
	}

private:
	CallbackMap _callbacks;
	ProtobufMessageCallback _defaultCallback;
};

}
}