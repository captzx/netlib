#pragma once

#include "Buffer.h"

#include <boost/asio.hpp>

namespace x {
namespace net {

using boost::asio::ip::tcp;
using SocketPtr = std::shared_ptr<tcp::socket>;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(SocketPtr);

	void Established(); 
	// void Disconnected();

	// void Close();
	// void Shutdown();

	// void SyncSend(const void* message, int len);
	void AsyncSend(unsigned int);
	// void SyncReceive();
	void AsyncReceive();
private:
	SocketPtr _pSock;
	Buffer _buf;
};

using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

} // namespace net
} // namespace x