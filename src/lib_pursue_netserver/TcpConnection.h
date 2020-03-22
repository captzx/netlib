#pragma once

#include <boost/asio/ip/tcp.hpp>

namespace piece {
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

	char _data[max_length];
};

typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

}
}