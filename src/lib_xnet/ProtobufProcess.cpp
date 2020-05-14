#include "ProtobufProcess.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;
using namespace google::protobuf;

/// ProtobufCodec
const static uint32_t HEADER_LEN_SIZE = sizeof(uint32_t);
const static uint32_t MESSAGE_NAME_LEN_SIZE = sizeof(uint32_t);
const static uint32_t CHECKSUM_LEN_SIZE = sizeof(uint32_t);
const static int MESSAGE_MIN_LEN = MESSAGE_NAME_LEN_SIZE + CHECKSUM_LEN_SIZE + 2; // MESSAGE_NAME_LEN_SIZE + CHECKSUM_LEN_SIZE + message name len(at least one char + '\n')
const static int MESSAGE_MAX_LEN = 64 * 1024 * 1024;

bool ProtobufCodec::PackMessage(const MessagePtr& pMessage, Buffer& buf) {
	if (!pMessage) return false;

	const std::string& messageName = pMessage->GetTypeName();
	uint32_t messageNameLen = static_cast<uint32_t>(messageName.size() + 1); // + '\n'
	buf.Write(messageNameLen);
	buf.Write(messageName.c_str(), messageNameLen);

	int messageLen = pMessage->ByteSizeLong();
	buf.EnsureWriteable(messageLen);
	uint8_t* start = reinterpret_cast<uint8_t*>(buf.WritePtr());
	uint8_t* end = pMessage->SerializeWithCachedSizesToArray(start);
	if (end - start != messageLen) {
		log(error) << "[ProtobufCodec]pack message failure, error message: message serialize len != massage len.";
		return false;
	}
	buf.MoveWritePtr(messageLen);

	int checkSum = static_cast<int>(::adler32(1, reinterpret_cast<const Bytef*>(buf.ReadPtr()), static_cast<int>(buf.Readable())));
	buf.Write(checkSum);
	assert(buf.Readable() == MESSAGE_NAME_LEN_SIZE + messageNameLen + messageLen + CHECKSUM_LEN_SIZE);

	int32_t headerLen = buf.Readable();
	buf.Prepend(&headerLen, HEADER_LEN_SIZE);

	return true;
}

MessagePtr ProtobufCodec::ParseDataPackage(const char* buf, uint32_t dataLen) {
	int expectedCheckSum = 0;
	std::memcpy(&expectedCheckSum, buf + (dataLen - CHECKSUM_LEN_SIZE), CHECKSUM_LEN_SIZE);
	int checkSum = static_cast<int>(::adler32(1, reinterpret_cast<const Bytef*>(buf), static_cast<int>(dataLen - CHECKSUM_LEN_SIZE)));
	if (expectedCheckSum != checkSum) {
		log(error) << "[ProtobufCodec]parse data failure, error message: check failure! check sum = " << expectedCheckSum << " vs "<< checkSum;
		return nullptr;
	}

	uint32_t nameLen = 0;
	std::memcpy(&nameLen, buf, MESSAGE_NAME_LEN_SIZE);
	if (nameLen < 2 || nameLen > dataLen - (MESSAGE_NAME_LEN_SIZE + CHECKSUM_LEN_SIZE)) {
		log(error) << "[ProtobufCodec]parse data failure, error message: name len too short(or long)! nameLen =" << nameLen;
		return nullptr;
	}
	std::string messageName(buf + MESSAGE_NAME_LEN_SIZE, buf + MESSAGE_NAME_LEN_SIZE + nameLen - 1);
	MessagePtr pMessage = CreateMessageByName(messageName);
	if (pMessage) {
		const char* messageData = buf + MESSAGE_NAME_LEN_SIZE + nameLen;
		int messageLen = dataLen - (nameLen + MESSAGE_NAME_LEN_SIZE + CHECKSUM_LEN_SIZE);
		if (pMessage->ParseFromArray(messageData, messageLen)) {
			log(debug) << "[ProtobufCodec]parse data success, get message: \"" << messageName << "\"";
		}
	}

	return pMessage;
}
MessagePtr ProtobufCodec::CreateMessageByName(const std::string& messageName) {
	MessagePtr pMessage;
	const Descriptor* descriptor = DescriptorPool::generated_pool()->FindMessageTypeByName(messageName);
	if (descriptor) {
		const Message* prototype = MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype) pMessage.reset(prototype->New());
	}

	return pMessage;
}

void ProtobufCodec::RecvMessage(const TcpConnectionPtr& pConnection, Buffer& buf) {
	while (buf.Readable() >= MESSAGE_MIN_LEN + HEADER_LEN_SIZE) {
		const uint32_t messageLen = buf.Peek<uint32_t>();
		if (messageLen > MESSAGE_MAX_LEN || messageLen < MESSAGE_MIN_LEN) break;
		
		if (buf.Readable() >= static_cast<size_t>(messageLen + HEADER_LEN_SIZE)) {
			MessagePtr message = ParseDataPackage(buf.ReadPtr() + HEADER_LEN_SIZE, messageLen);
			_messageCallback(pConnection, message);

			buf.Retrieve(HEADER_LEN_SIZE + messageLen);
		}
	}
}

/// ProtobufDispatcher
void ProtobufDispatcher::RecvMessage(const TcpConnectionPtr& pConnection, const MessagePtr& pMessage) const {
	if (!pMessage) return;

	auto it = _callbacks.find(pMessage->GetDescriptor());
	if (it != _callbacks.end())
		it->second->OnMessage(pConnection, pMessage);
	else
		_defaultCallback(pConnection, pMessage);
}