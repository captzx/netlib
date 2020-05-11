#include "TcpService.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

using namespace std::placeholders;

void x::net::DefaultMessageCallback(const TcpConnectionPtr&, Buffer&) {
	log(debug) << "default message call back.";
}
void x::net::DefaultConnectionCallback(const TcpConnectionPtr&) {
	log(debug) << "default connection call back.";
}

void IOContext::Run() {
	_io_context.run();
}
void IOContext::RunInThread() {
	_ctx_running = std::thread([this] { _io_context.run(); });
}

io_context& IOContext::Get() { 
	return _io_context; 
}

void IOContext::Stop() {
	_io_context.stop();
	_ctx_running.join();
}

/// TcpConnection
TcpConnection::TcpConnection(tcp::socket sock) :
	_sock(std::move(sock)),
	_messageCallback(std::bind(&DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)) {
}

void TcpConnection::Established() {
	log(debug) << "tcp make establish, remote endPoint: " << _sock.remote_endpoint() << ".";
	log(debug) << "socket is non-blocking mode? " << _sock.non_blocking() << ".";
	log(debug) << "socket is open? " << _sock.is_open() << ".";
	
	auto self(shared_from_this());
	_sock.async_wait(tcp::socket::wait_read, // Asynchronously wait for the socket to become ready to read, ready to write, or to have pending error conditions.
		[this, self](const error_code& code) -> void {
			if (code) {
				log(error) << "async wait failure, error message: " << code.message();
				return;
			}

			AsyncReceive();
		}
	);
}

void TcpConnection::AsyncReceive() {
	auto self(shared_from_this());
	_sock.async_read_some(asio::buffer(_buf.WritePtr(), _buf.Writeable()),
		[this, self](const error_code& code, size_t len)  -> void { // NOTICE: lambda capture [this, self]
			if (len > 0) {
				this->_buf.MoveWritePtr(len);
				_messageCallback(shared_from_this(), this->_buf);
				AsyncReceive();
				log(debug) << "async receive data from: " << _sock.remote_endpoint() << ", length: " << len;
			}
			else if (len == 0) {
				// _closeCallback(shared_from_this(), this->_buf);
				log(debug) << _sock.remote_endpoint() << " close.";
			}
			else if (code) {
				log(error) << "async receive failure, error message: " << code.message();
			}
		});
}

void TcpConnection::AsyncSend(Buffer& buf) {
	auto self(shared_from_this());
	_sock.async_write_some(asio::buffer(buf.ReadPtr(), buf.Readable()),
		[this, self](const error_code& code, size_t len) -> void {
			if (code) {
				log(error) << "async send failure, error message: " << code.message();
				return;
			}

			log(debug) << "async send data, len: " << len;
		});
}

void TcpConnection::Close() {
	_sock.close();

	log(normal) << "tcp connection close.";
}

/// TcpServer
int connection_count = 0;

TcpServer::TcpServer(IOContext& ctx, std::string name): TcpServices(name),
	_acceptor(ctx.Get()) {
	
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

	boost::asio::socket_base::linger option;
	_acceptor.get_option(option);
	bool is_set = option.enabled();
	if (!is_set) {
		boost::asio::socket_base::linger option(true, 0);
		_acceptor.set_option(option);
		log(debug) << "linger: open?: " << option.enabled() << ", time_out" << option.timeout();;
	}

	std::this_thread::sleep_for(std::chrono::seconds(5));

	log(normal) << "acceptor listening... port: " << port;

	AsyncListenInLoop();
}

void TcpServer::AsyncListenInLoop() {
	SocketPtr pSock = std::make_shared<tcp::socket>(_acceptor.get_executor());
	
	_acceptor.async_accept(*pSock, [this, pSock](const error_code& code) { // pSock: capture by value, keep alive
			if (code) {
				log(error) << "async accept failure, error message: " << code.message();
				return;
			}

			TcpConnectionPtr pConnection = std::make_shared<TcpConnection>(std::move(*pSock));
			pConnection->SetMessageCallback(_messageCallback);
			_tcpConnections.insert({connection_count++, pConnection});
			pConnection->Established();

			_connectionCallback(pConnection);

			AsyncListenInLoop();
		}
	);
}

/// TcpClient
TcpClient::TcpClient(IOContext& ctx, std::string name) : 
	TcpServices(name),
	_pConnection(std::make_shared<TcpConnection>(std::move(tcp::socket(ctx.Get())))) {

}

void TcpClient::AsyncConnect(std::string ip, unsigned int port) {
	tcp::endpoint endpoint = tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
	_pConnection->GetSocket().async_connect(endpoint,
		[this](const error_code& code) {
		if (code) {
			log(error) << "connect failure, error message: " << code.message();
			return;
		}

		_pConnection->SetMessageCallback(_messageCallback);
		_pConnection->Established();
		}
	);
}

void TcpClient::Close() {
	if (_pConnection) _pConnection->Close();
}

void TcpClient::AsyncSend(Buffer & buf) {
	if (_pConnection) _pConnection->AsyncSend(buf);
}
