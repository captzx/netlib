#include "common.h"

#include "NetMoudle.h"

using boost::asio::ip::tcp;

NetMoudle::NetMoudle():
	_acceptor(_io_context) {

}

void NetMoudle::Init() {
	log(normal) << "[NetMoudle]Init!";


}
void NetMoudle::Listen(unsigned int port) {
	if (port == 0) {
		log(error) << "[NetMoudle]Listening... Port: " << port;
		return;
	}

	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::tcp::v4(), port);
	_acceptor.open(ep.protocol());
	boost::system::error_code ec;
	_acceptor.bind(ep, ec);
	if (ec) {
		log(error) << "[NetMoudle]Listening... Exception" << ec;
		return;
	}

	log(normal) << "[NetMoudle]Listening... Port: " << port;

	_Listening();
	_io_context.run();
}


void NetMoudle::_Listening() {
	_acceptor.async_accept(
		[&](const boost::system::error_code& ec, tcp::socket sock)
		{
			if (!ec)
			{
				log(debug) << "[NetMoudle]New Tcp Connect, Remote EndPoint: " << sock.local_endpoint();
				std::make_shared<TcpConnection>(std::move(sock))->start();
			}

			_Listening();
		});
}

TcpConnection::TcpConnection(tcp::socket&& sock):
	_sock(std::move(sock)) {

}

void TcpConnection::start() {
	do_read();
}

void TcpConnection::do_read() {
	auto self(shared_from_this());
	_sock.async_read_some(boost::asio::buffer(_data, max_length),
		[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) { // NOTICE: lambda capture [this, self]
			if (!ec) do_write(bytes_transferred);

			std::cout << bytes_transferred << std::endl;
		}
	);
}

void TcpConnection::do_write(size_t bytes_transferred) {
	auto self(shared_from_this());
	_sock.async_write_some(boost::asio::buffer(_data, bytes_transferred),
		[this, self](const boost::system::error_code& ec, std::size_t bytes_transferred) {
			if (!ec) do_read();

			std::cout << bytes_transferred << std::endl;
		}
	);
}