#pragma once 

#include <boost/asio.hpp>

namespace x {

namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_context;

class TcpConnection;
using TcpConnectionManager = std::map<int, std::shared_ptr<TcpConnection>>;

class NetServer {
public:
	NetServer();

public:
	void Init();
	void Listen(unsigned int port);


private:
	void _Listening();

private:
	io_context _io_context;
	tcp::acceptor _acceptor;

	TcpConnectionManager _tcpConnections;
};

} // namespace net

} // namespace x