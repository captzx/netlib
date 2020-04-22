#include "TcpConnection.h"

#include <xtools/Logger.h>
#include <boost/asio.hpp>

#include "ProtobufCodec.h"

using boost::asio::ip::tcp;
using boost::system::error_code;
using namespace boost;
using namespace x::net;
using namespace x::tool;

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
	_sock->async_read_some(asio::buffer(_buf.WritePtr(), _buf.Writeable()),
		[this, self](const error_code& ec, size_t len) { // NOTICE: lambda capture [this, self]
			if (!ec) {
				this->_buf.MoveWritePtr(len);
				ProtobufCodec::GetInstance().Recv(shared_from_this(), &this->_buf);
			}

			if (len > 0)
				log(debug) << "read data from " << _sock->remote_endpoint() << ", length: " << len;
			else if (len == 0)
				log(debug) << _sock->remote_endpoint() << " close.";
		});
}

void TcpConnection::AsyncSend(unsigned int len) {
	auto self(shared_from_this());
	_sock->async_write_some(asio::buffer(_buf.ReadPtr(), len),
		[this, self](const error_code& ec, size_t len) {
			if (!ec) AsyncReceive();

			log(debug) << "write data to " << _sock->remote_endpoint() << ", length: " << len;
		});
}