#include "common.h"
#include "TcpConnection.h"

#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using boost::system::error_code;
using namespace boost;
using namespace piece::net;
using namespace piece::tool;

TcpConnection::TcpConnection(std::shared_ptr<tcp::socket> sock):
	_sock(sock) {
}

void TcpConnection::Established() {
	log(debug) << "tcp make establish, remote endPoint: " << _sock->remote_endpoint() << ". " 
		<< "socket is non-blocking mode? " << _sock->non_blocking() << ". "
		<< "socket is open? " << _sock->is_open() << ".";

	auto self(shared_from_this());
	_sock->async_wait(tcp::socket::wait_read, 
		[this, self](const error_code& ec) {
			if (!ec) {
				AsyncReceive();
			}
		});
}

void TcpConnection::AsyncReceive() {
	auto self(shared_from_this());
	_sock->async_read_some(asio::buffer(_data, max_length),
		[this, self](const error_code& ec, size_t bytes_transferred) { // NOTICE: lambda capture [this, self]
			if (!ec) AsyncSend(bytes_transferred);

			if (bytes_transferred > 0)
				log(debug) << "read data from " << _sock->remote_endpoint() << ", length: " << bytes_transferred;
			else if (bytes_transferred == 0)
				log(debug) << _sock->remote_endpoint() << " close.";
		});
}
void TcpConnection::AsyncSend(unsigned int bytes_transferred) {
	auto self(shared_from_this());
	_sock->async_write_some(asio::buffer(_data, bytes_transferred),
		[this, self](const error_code& ec, size_t bytes_transferred) {
			if (!ec) AsyncReceive();

			log(debug) << "write data to " << _sock->remote_endpoint() << ", length: " << bytes_transferred;
		});
}