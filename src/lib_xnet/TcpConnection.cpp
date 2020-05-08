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

	AsyncReceive();
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
			_messageCallback(shared_from_this(), this->_buf);
			AsyncReceive();
		});
}

//inline void x::net::TcpConnection::DefaultMessageCallback(const TcpConnectionPtr&, Buffer&) {
//	log(debug) << "tcp connection default message call back.";
//}

void TcpConnection::AsyncSend(Buffer& buf) {
	auto self(shared_from_this());
	_pSock->async_write_some(asio::buffer(buf.ReadPtr(), buf.Readable()),
		[this, self](const error_code& code, size_t len) -> void {
			if (code) {
				log(error) << "async send failure, error message: " << code.message();
				return;
			}

			log(debug) << "async send data, len: " << len;
		});
}

void TcpConnection::Close() {
	_pSock->close();

	log(normal) << "tcp connection close.";
}