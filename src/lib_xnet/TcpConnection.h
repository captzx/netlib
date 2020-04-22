#pragma once

#include <boost/asio.hpp>

#include "Buffer.h"

namespace google::protobuf {
	class Message;
}

namespace x {
namespace net {

const unsigned int max_length = 1024;

using boost::asio::ip::tcp;


class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(std::shared_ptr<tcp::socket> sock);

	void Established(); 
	// void Disconnected();

	// void Close();
	// void Shutdown();

	// void SyncSend(const void* message, int len);
	void AsyncSend(unsigned int);
	// void SyncReceive();
	void AsyncReceive();
private:
	std::shared_ptr<tcp::socket> _sock;

	Buffer _buf;
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}
}