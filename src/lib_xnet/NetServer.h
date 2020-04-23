#pragma once 

#include <boost/asio.hpp>
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

private:
	void AsyncListenInLoop();
private:
	std::string _name;
	io_context _io_context;
	tcp::acceptor _acceptor;
	TcpConnectionManager _tcpConnections;
};

} // namespace net

} // namespace x