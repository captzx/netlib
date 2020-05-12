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
bool IOContext::IsRunning() {
	return _io_context.stopped() == false;
}
void IOContext::Stop() {
	if (_ctx_running.joinable()) {
		_ctx_running.join();
	}
}

/// TcpConnection
TcpConnection::TcpConnection(tcp::socket sock) :
	_id(0),
	_sock(std::move(sock)),
	_messageCallback(std::bind(&DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)) {
}

TcpConnection::~TcpConnection() {
	log(debug) << "~TcpConnection::TcpConnection";
}

void TcpConnection::Established() {
	log(debug) << "tcp make establish, remote end point: " << _sock.remote_endpoint() << ".";
	log(debug) << "socket is non-blocking mode? " << _sock.non_blocking() << ".";
	log(debug) << "socket is open? " << _sock.is_open() << ".";
	
	_sock.async_wait(tcp::socket::wait_read, // Asynchronously wait for the socket to become ready to read, ready to write, or to have pending error conditions.
		[this](const error_code& code) -> void {
			if (code) {
				log(error) << "async wait failure, error message: " << code.message();
				return;
			}

			AsyncReceive();
		}
	);
}

void TcpConnection::AsyncReceive() {
	_sock.async_read_some(asio::buffer(_buf.WritePtr(), _buf.Writeable()),
		[this](const error_code& code, size_t len)  -> void { // NOTICE: lambda capture [this, self]
			if (code) {
				log(error) << "async receive failure, error message: " << code.message();
				Disconnect();
				return;
			}

			if (len > 0) {
				this->_buf.MoveWritePtr(len);
				_messageCallback(shared_from_this(), this->_buf);
				AsyncReceive();
				log(debug) << "async receive data from: " << _sock.remote_endpoint() << ", length: " << len;
			}
			else if (len == 0) {
				Disconnect();
			}
		});
}

void TcpConnection::AsyncSend(Buffer& buf) {
	_sock.async_write_some(asio::buffer(buf.ReadPtr(), buf.Readable()),
		[this](const error_code& code, size_t len) -> void {
			if (code) {
				log(error) << "async send failure, error message: " << code.message();
				return;
			}

			log(debug) << "async send data, len: " << len;
		});
}

void TcpConnection::Disconnect() {
	log(debug) << "tcp disconnect, remote end point: " << _sock.remote_endpoint() << ".";
	_sock.cancel(); // mark
	_sock.shutdown(tcp::socket::shutdown_both);
	_sock.close();
	_closeCallback(shared_from_this());
}

/// TcpServer
TcpServer::TcpServer(IOContext& ctx, std::string name): TcpServices(name),
	_counter(0),
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

void TcpServer::RemoveConnection(const TcpConnectionPtr& pConnection) {
	// TcpConnectionPtr pGuard(pConnection);

	auto it = _tcpConnections.find(pConnection->GetID());
	if (it != _tcpConnections.end()) {
		assert(pConnection == it->second);

		_tcpConnections.erase(pConnection->GetID());
	}
}

void TcpServer::AsyncListenInLoop() {
	SocketPtr pSock = std::make_shared<tcp::socket>(_acceptor.get_executor());
	
	_acceptor.async_accept(*pSock, [this, pSock](const error_code& code) { // pSock: capture by value, keep alive
			if (code) {
				log(error) << "async accept failure, error message: " << code.message();
				return;
			}

			TcpConnectionPtr pConnection = std::make_shared<TcpConnection>(std::move(*pSock));
			pConnection->SetID(_counter);
			pConnection->SetMessageCallback(_messageCallback);
			pConnection->SetCloseCallback(std::bind(&TcpServer::RemoveConnection, this, std::placeholders::_1));
			pConnection->Established();
			_tcpConnections.insert({ _counter++, pConnection });

			_connectionCallback(pConnection);

			AsyncListenInLoop();
		}
	);
}

/// TcpClient
TcpClient::TcpClient(IOContext& ctx, std::string name) :
	TcpServices(name),
	_ctx(ctx),
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
		_pConnection->SetCloseCallback(std::bind(&TcpClient::DestroyConnection, this, std::placeholders::_1));
		_pConnection->Established();
		}
	);
}

void TcpClient::Close() {
	if (_pConnection) _pConnection->Disconnect();
}

void TcpClient::DestroyConnection(const TcpConnectionPtr& pConnection) {
	assert(pConnection == _pConnection);

	_pConnection.reset();

	log(debug) << "removed connection, strong ref: " << pConnection.use_count();
}

void TcpClient::AsyncSend(Buffer & buf) {
	if (_pConnection) _pConnection->AsyncSend(buf);
}
