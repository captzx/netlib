#include "TcpService.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

using namespace std::placeholders;

void DefaultMessageCallback(const TcpConnectionPtr&, Buffer&) {
	log(debug) << "tcp connection default message call back.";
}

/// TcpConnection
using boost::system::error_code;

TcpConnection::TcpConnection(SocketPtr&& pSock) :
	_pSock(pSock),
	_messageCallback(std::bind(DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)) {
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

/// TcpServer
int connection_count = 0;

TcpServer::TcpServer(std::string name): TcpServices(name),
	_acceptor(_io_context) {
	
}

void TcpServer::Listen(unsigned int port) {
	if (port == 0) {
		log(error) << "acceptor listen failure, error message: port = " << port;
		return;
	}

	tcp::endpoint local(tcp::v4(), port);
	_acceptor.open(local.protocol());

	error_code code;

	_acceptor.bind(local, code);
	if (code) {
		log(error) << "acceptor bind port " << port << " failure, error message: " << code.message();
		return;
	}

	_acceptor.listen(asio::socket_base::max_listen_connections, code);
	if (code) {
		log(error) << "acceptor listen port " << port << " failure, error code: " << code.message();
		return;
	}

	log(normal) << "acceptor listening... port: " << port;

	AsyncListenInLoop();

	_io_context.run();
}

void TcpServer::AsyncListenInLoop() {
	SocketPtr pSock  = std::make_shared<tcp::socket>(_io_context);
	_acceptor.async_accept(*pSock,
		[this, pSock](const error_code& code)
		{
			if (code) {
				log(error) << "async accept failure, error message: " << code.message();
				return;
			}

			TcpConnectionPtr pConnection = std::make_shared<TcpConnection>(pSock);
			pConnection->SetMessageCallback(_messageCallback);
			_tcpConnections.insert({connection_count++, pConnection});
			pConnection->Established();

			_connectionCallback(pConnection);

			AsyncListenInLoop();
		});
}

/// TcpClient
TcpClient::TcpClient(std::string name) :
	_name(name) {

}

void TcpClient::AsyncConnect(tcp::endpoint end) {
	SocketPtr pSock = std::make_shared<tcp::socket>(_io_context);
	pSock->async_connect(end, [this, pSock](const error_code& code) {
		if (code) {
			log(error) << "connect failure, error message: " << code.message();
			return;
		}

		_pConnection = std::make_shared<TcpConnection>(std::move(pSock));
		_pConnection->SetMessageCallback(_messageCallback);
		_pConnection->Established();
		log(error) << "tcp client connect success.";
		}
	);

	std::thread([&] { _io_context.run(); }).detach();
}

void TcpClient::Close() {
	if (_pConnection) _pConnection->Close();
}

void TcpClient::AsyncSend(Buffer & buf) {
	if (_pConnection) _pConnection->AsyncSend(buf);
}
