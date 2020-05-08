#pragma once 

#include "Using.h"
#include "Buffer.h"
#include "TcpConnection.h"

namespace x {

namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_context;

using TcpConnectionManager = std::map<int, TcpConnectionPtr>;

class NetServer {
public:
	NetServer(std::string);

public:
	void Listen(unsigned int port);

public:
	void SetMessageCallback(MessageCallback callback) {
		_messageCallback = callback;
	}
	//void DefaultMessageCallback(const TcpConnectionPtr&, Buffer&);

private:
	void AsyncListenInLoop();

private:
	std::string _name;
	io_context _io_context;
	tcp::acceptor _acceptor;
	TcpConnectionManager _tcpConnections;

private:
	MessageCallback _messageCallback;
};

} // namespace net

} // namespace x