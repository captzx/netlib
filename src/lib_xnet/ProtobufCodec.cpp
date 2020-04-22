#include "ProtobufCodec.h"

#include <xtools/Logger.h>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include <zlib.h>

#include "Buffer.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;

void ProtobufCodec::PackMessage(Buffer& buf, const google::protobuf::Message& message)
{
	const std::string& message_name = message.GetTypeName();
	int32_t message_name_len = static_cast<int32_t>(message_name.size() + 1);

	buf.Write(message_name_len);
	buf.Write(message_name.c_str(), message_name_len);

	int message_size = message.ByteSizeLong();
	buf.EnsureWriteable(message_size);

	uint8_t* start = reinterpret_cast<uint8_t*>(buf.WritePtr());
	uint8_t* end = message.SerializeWithCachedSizesToArray(start);
	if (end - start != message_size)
	{
		log(debug) << "PackMessage error";
	}

	buf.MoveWritePtr(message_size);

	int32_t checkSum = static_cast<int32_t>(::adler32(1,reinterpret_cast<const Bytef*>(buf.ReadPtr()),static_cast<int>(buf.Readable())));
	buf.Write(checkSum);

	assert(buf.Readable() == sizeof message_name_len + message_name_len + message_size + sizeof checkSum);

	// int32_t len = sockets::hostToNetwork32(static_cast<int32_t>(buf->readableBytes()));
	size_t len = buf.Readable();

	buf.Prepend(&len, sizeof len);
}

MessagePtr ProtobufCodec::Parse(const char* buf, int len) {
	MessagePtr message;

	int32_t expectedCheckSum = 0;
	std::memcpy(&expectedCheckSum, buf + len - header_len, header_len);
	int32_t checkSum = static_cast<int32_t>(::adler32(1, reinterpret_cast<const Bytef*>(buf), static_cast<int>(len - header_len)));

	if (expectedCheckSum == checkSum) {
		int32_t message_name_len = 0;
		std::memcpy(&message_name_len, buf, header_len);
		if (message_name_len >= 2 && message_name_len <= len - 2 * header_len)
		{
			std::string message_name(buf + header_len, buf + header_len + message_name_len - 1);
			message.reset(CreateMessage(message_name));
			if (message)
			{
				const char* data = buf + header_len + message_name_len;
				int32_t dataLen = len - message_name_len - 2 * header_len;
				if (message->ParseFromArray(data, dataLen))
				{
					log(debug) << "Message Parse Success!";
				}
			}
		}
	}

	return message;
}

google::protobuf::Message* ProtobufCodec::CreateMessage(const std::string& message_name)
{
	google::protobuf::Message* message = NULL;
	const google::protobuf::Descriptor* descriptor =
		google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(message_name);
	if (descriptor)
	{
		const google::protobuf::Message* prototype =
			google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
		if (prototype)
		{
			message = prototype->New();
		}
	}
	return message;
}

void ProtobufCodec::Recv(const TcpConnectionPtr& conn, Buffer* buf) {
	log(debug) << "Recv data, cur buf readable: " << buf->Readable();
	while (buf->Readable() >= message_min_len + header_len)
	{
		const int32_t len = buf->Read<int32_t>();
		if (len > message_max_len || len < message_min_len)
		{
			break;
		}
		else if (buf->Readable() >= static_cast<size_t>(len + header_len))
		{
			MessagePtr message = Parse(buf->ReadPtr() + header_len, len);
			// messageCallback_(conn, message, receiveTime);
			buf->Retrieve(header_len + len);
		}
		else break;
	}
}

void ProtobufCodec::Send(const TcpConnectionPtr& conn, const google::protobuf::Message& message)
{
	Buffer buf;
	PackMessage(buf, message);
	// conn->send(&buf);
}