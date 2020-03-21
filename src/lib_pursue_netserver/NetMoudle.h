#pragma once 

#include <boost/asio.hpp>

const unsigned int max_length = 1024;

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(boost::asio::ip::tcp::socket&& sock);

	void start();

private:
	void do_read();
	void do_write(size_t bytes_transferred);

private:
	boost::asio::ip::tcp::socket _sock;

	char _data[max_length];
};

class NetMoudle {
public:
	NetMoudle();

public:
	void Init();
	void Listen(unsigned int port);


private:
	void _Listening();

private:
	boost::asio::io_context _io_context;
	boost::asio::ip::tcp::acceptor _acceptor;
};