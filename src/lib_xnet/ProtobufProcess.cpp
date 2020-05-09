#include "ProtobufProcess.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;
using namespace google::protobuf;

/// ProtobufCodec
const static int header_len = sizeof(int32_t);
const static int message_min_len = 2 * header_len + 2; // nameLen + checkSum + message name(at least one char + '\n')
const static int message_max_len = 64 * 1024 * 1024;

void ProtobufCodec::PackMessage(const MessagePtr& pMessage, Buffer& buf) {
	if (!pMessage) return;

	const std::string& message_name = pMessage->GetTypeName();
	int32_t message_name_len = static_cast<int32_t>(message_name.size() + 1);
	buf.Write(message_name_len);
	buf.Write(message_name.c_str(), message_name_len);

	int message_size = pMessage->ByteSizeLong();
	buf.EnsureWriteable(message_size);
	uint8_t* start = reinterpret_cast<uint8_t*>(buf.WritePtr());
	uint8_t* end = pMessage->SerializeWithCachedSizesToArray(start);
	if (end - start != message_size) log(error) << "pack message error, message serialize size not equal.";
	buf.MoveWritePtr(message_size);

	int32_t checkSum = static_cast<int32_t>(::adler32(1, reinterpret_cast<const Bytef*>(buf.ReadPtr()), static_cast<int>(buf.Readable())));
	buf.Write(checkSum);
	assert(buf.Readable() == sizeof message_name_len + message_name_len + message_size + sizeof checkSum);

	size_t len = buf.Readable();
	buf.Prepend(&len, sizeof len);
}

MessagePtr ProtobufCodec::ParseDataPackage(const char* buf, int len) {
	MessagePtr pMessage;

	int32_t expectedCheckSum = 0;
	std::memcpy(&expectedCheckSum, buf + len - header_len, header_len);
	int32_t checkSum = static_cast<int32_t>(::adler32(1, reinterpret_cast<const Bytef*>(buf), static_cast<int>(len - header_len)));
	if (expectedCheckSum != checkSum) {
		log(error) << "protobuf codec parse failure, checksum not correct!";
		return pMessage;
	}
	
	int32_t message_name_len = 0;
	std::memcpy(&message_name_len, buf, header_len);
	if (message_name_len < 2 || message_name_len > len - 2 * header_len) {
		log(error) << "protobuf codec parse failure, message name too short(or long)!";
		return pMessage;
	}
	std::string message_name(buf + header_len, buf + header_len + message_name_len - 1);
	pMessage = CreateMessageByName(message_name);
	if (pMessage) {
		const char* data = buf + header_len + message_name_len;
		int32_t data_len = len - message_name_len - 2 * header_len;
		if (pMessage->ParseFromArray(data, data_len)) log(debug) << "protobuf codec parse success, get message: \"" << message_name << "\"";
	}

	return pMessage;
}
MessagePtr ProtobufCodec::CreateMessageByName(const std::string& message_name){
	MessagePtr pMessage;
	const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(message_name);
	if (descriptor){
		const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype) pMessage.reset(prototype->New());
	}

	return pMessage;
}

void ProtobufCodec::RecvMessage(const TcpConnectionPtr& pConnection, Buffer& buf) {
	log(debug) << "recv data, cur buf readable: " << buf.Readable();

	while (buf.Readable() >= message_min_len + header_len)
	{
		const int32_t len = buf.Read<int32_t>();
		if (len > message_max_len || len < message_min_len)
		{
			break;
		}

		else if (buf.Readable() >= static_cast<size_t>(len + header_len))
		{
			MessagePtr message = ParseDataPackage(buf.ReadPtr() + header_len, len);
			_messageCallback(pConnection, message);

			buf.Retrieve(header_len + len);

		}
		else break;
	}
}

void ProtobufCodec::SendMessage(const TcpConnectionPtr& pConnection, const MessagePtr& pMessage) {
	if (!pConnection || pMessage) return;

	Buffer buf;
	PackMessage(pMessage, buf);
	pConnection->AsyncSend(buf);
}

/// ProtobufDispatcher
void ProtobufDispatcher::RecvMessage(const TcpConnectionPtr& pConnection, const MessagePtr& pMessage) const{
	if (!pMessage) return;

	auto it = _callbacks.find(pMessage->GetDescriptor());
	if (it != _callbacks.end()) 
		it->second(pConnection, pMessage);
	else 
		_defaultCallback(pConnection, pMessage);
}

void ProtobufDispatcher::RegisterMessageCallback(const Descriptor* pDesc, const ProtobufMessageCallback& callback){
	if (pDesc) _callbacks[pDesc] = callback;
}
