#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>

#include <cryptopp/adler32.h>

#include "Using.h"
#include "Buffer.h"
#include "TcpService.h"

using google::protobuf::Message;
using google::protobuf::Descriptor;

namespace x {

namespace net {

using MessagePtr = std::shared_ptr<Message>;
using ProtobufMessageCallback = std::function<void(const TcpConnectionPtr&, const MessagePtr&)>;

/// ProtobufCodec
class ProtobufCodec {
public:
	ProtobufCodec(const ProtobufMessageCallback& callback) {
		_messageCallback = callback;
	}
public:
	static bool PackMessage(const MessagePtr&, Buffer&);
	static MessagePtr CreateMessageByName(const std::string&);
	static MessagePtr ParseDataPackage(const char*, uint32_t);
	void RecvMessage(const TcpConnectionPtr&, Buffer&);

private:
	static CryptoPP::Adler32 _adler32;
	ProtobufMessageCallback _messageCallback;
};

/// ProtobufMessage Callback Interface
class Callback : public  x::tool::NoCopyable {
public:
	virtual ~Callback() = default;
	virtual void OnMessage(const TcpConnectionPtr&, const MessagePtr&) const = 0;
};

/// ProtobufMessage Callback Template
template <typename T>
class ProtobufMessageCallbackT : public Callback {
private:
	static_assert(std::is_base_of<Message, T>::value, "T must be derived from google::protobuf::Message.");

public:
	using CallbackType = std::function<void(const TcpConnectionPtr&, const std::shared_ptr<T>&)>;

	ProtobufMessageCallbackT(const CallbackType& callback) : _callback(callback)
	{
	}

	void OnMessage(const TcpConnectionPtr& pConnection, const MessagePtr& pMessagee) const override {
		std::shared_ptr<T> pTMessage = std::dynamic_pointer_cast<T>(pMessagee);

		_callback(pConnection, pTMessage);
	}

private:
	CallbackType _callback;
};

/// ProtobufDispatcher
class ProtobufDispatcher
{
public:
	explicit ProtobufDispatcher(const ProtobufMessageCallback& callback) {
		_defaultCallback = callback;
	}

public:
	void RecvMessage(const TcpConnectionPtr&, const MessagePtr&) const;

	template<typename T>
	void RegisterMessageCallback(const typename ProtobufMessageCallbackT<T>::CallbackType& callback) {
		std::shared_ptr<ProtobufMessageCallbackT<T>> pCallback(new ProtobufMessageCallbackT<T>(callback));
		_callbacks[T::descriptor()] = pCallback;
	}

private:
	using ProtobufMessageCallbackMap = std::map<const Descriptor*, std::shared_ptr<Callback>>;
	ProtobufMessageCallbackMap _callbacks;
	ProtobufMessageCallback _defaultCallback;
};

}

}
