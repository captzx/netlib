#include "TcpConnection.h"

#include "ProtobufCodec.h"

#include <xtools/Logger.h>

using namespace x::net;
using namespace x::tool;
using namespace boost;

using boost::system::error_code;

TcpConnection::TcpConnection(SocketPtr pSock):
	_pSock(pSock) {
}

void TcpConnection::Established() {
	log(debug) << "tcp make establish, remote endPoint: " << _pSock->remote_endpoint() << ".";
	log(debug) << "socket is non-blocking mode? " << _pSock->non_blocking() << ".";
	log(debug) << "socket is open? " << _pSock->is_open() << ".";

	auto self(shared_from_this());
	_pSock->async_wait(tcp::socket::wait_read, // Asynchronously wait for the socket to become ready to read, ready to write, or to have pending error conditions.
		[this, self](const error_code& code) -> void {
			if (code) {
				log(error) << "async wait failure, error message: " << code.message();
				return;
			}
			
			AsyncReceive();
		});
}

void TcpConnection::AsyncReceive() {
	auto self(shared_from_this());
	_pSock->async_read_some(asio::buffer(_buf.WritePtr(), _buf.Writeable()),
		[this, self](const error_code& code, size_t len)  -> void { // NOTICE: lambda capture [this, self]
			if (code) {
				log(error) << "async receive failure, error message: " << code.message();
				return;
			}

			if (len == 0) log(debug) << _pSock->remote_endpoint() << " close.";
			else log(debug) << "async receive data from: " << _pSock->remote_endpoint() << ", length: " << len;

			this->_buf.MoveWritePtr(len);
			ProtobufCodec::GetInstance().Recv(shared_from_this(), &this->_buf);
		});
}

void TcpConnection::AsyncSend(unsigned int len) {
	auto self(shared_from_this());
	_pSock->async_write_some(asio::buffer(_buf.ReadPtr(), len),
		[this, self](const error_code& code, size_t len) -> void {
			if (code) {
				log(error) << "async send failure, error message: " << code.message();
				return;
			}

			AsyncReceive();

			log(debug) << "async receive data to: " << _pSock->remote_endpoint() << ", length: " << len;
		});
}