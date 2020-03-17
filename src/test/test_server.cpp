#include "common.h"
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

const unsigned int max_length = 1024;

class tcp_connection : public std::enable_shared_from_this<tcp_connection>{
public:
	tcp_connection(tcp::socket&& sock) :_sock(std::move(sock)) {

	}

	void start() {
		do_read();
	}

private:
	void do_read() {
		auto self(shared_from_this());
		_sock.async_read_some(buffer(_data, max_length),
			[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) { // NOTICE: lambda capture [this, self]
				if (!ec) do_write(bytes_transferred);

				std::cout << bytes_transferred << std::endl;
			}
		);
	}

	void do_write(size_t bytes_transferred) {
		auto self(shared_from_this());
		_sock.async_write_some(buffer(_data, bytes_transferred),
			[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
				if (!ec) do_read();

				std::cout << bytes_transferred << std::endl;
			}
		);
	}

private:
	tcp::socket _sock;
	char _data[max_length];
};

class server {
public:
	server(io_context& ioc, unsigned short port) :
		_acceptor(ioc, tcp::endpoint(tcp::v4(), port)) {
		log(debug) << "local: " << _acceptor.local_endpoint() << "listening..." <<  std::endl;
		do_accept();
	}

private:
	void do_accept() {
		_acceptor.async_accept(
			[&](const boost::system::error_code& ec, tcp::socket sock)
			{
				if (!ec)
				{
					log(debug) << "new tcp connect, remote: " << sock.local_endpoint() << std::endl;

					std::make_shared<tcp_connection>(std::move(sock))->start();
				}

				do_accept();
			});
	}

private:
	tcp::acceptor _acceptor;
};

int main(int argc, char* argv[])
{
	global_logger_init();

	io_context io_context;

	server s(io_context, std::atoi(argv[1]));

	io_context.run();

	return 0;
}