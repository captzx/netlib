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
void x::net::DefaultCloseCallback(const TcpConnectionPtr&) {
	log(debug) << "default connection call back.";
}

/// TcpConnection
int TcpConnection::Count = 0;

TcpConnection::TcpConnection(tcp::socket sock) : _sock(std::move(sock)),
	_id(TcpConnection::Count++),
	_state(Unused),
	_lastHeartBeat(0),
	_messageCallback(std::bind(&DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)),
	_closeCallback(std::bind(&DefaultCloseCallback, std::placeholders::_1)) {
}

TcpConnection::~TcpConnection() {
	log(debug, "TcpConnection") << "~TcpConnection::TcpConnection";
}

void TcpConnection::OnEstablish() {
	log(debug, "TcpConnection") << "tcp make establish, remote end point: " << _sock.remote_endpoint() << ".";
	log(debug, "TcpConnection") << "socket is non-blocking mode? " << _sock.non_blocking() << ".";
	log(debug, "TcpConnection") << "socket is open? " << _sock.is_open() << ".";

	_state = Connected;

	_sock.async_wait(tcp::socket::wait_read, // Asynchronously wait for the socket to become ready to read, ready to write, or to have pending error conditions.
		[this](const error_code& code) -> void {
			if (code) {
				log(error, "TcpConnection") << "async wait failure, error message: " << code.message();
				return;
			}

			AsyncReceive();
		}
	);
}

void TcpConnection::AsyncReceive() {
	_sock.async_read_some(asio::buffer(_buf.WritePtr(), _buf.Writeable()),
		[this](const error_code& code, size_t len)  -> void {
			if (len > 0) {
				this->_buf.MoveWritePtr(len);
				_messageCallback(shared_from_this(), this->_buf);
				AsyncReceive();

				log(trivial, "TcpConnection") << "async recv data, len: " << len;
			}
			else if (len == 0) {
				Disconnect();
			}
			else if (code) {
				log(error, "TcpConnection") << "async recv failure, error message: " << code.message();
				Disconnect();
			}
		});
}

void TcpConnection::AsyncSend(const MessagePtr& pMessage) {
	Buffer buf; ProtobufCodec::PackMessage(pMessage, buf);
	_sock.async_write_some(asio::buffer(buf.ReadPtr(), buf.Readable()),
		[this](const error_code& code, size_t len) -> void {
			if (code) {
				log(error, "TcpConnection") << "async send failure, error message: " << code.message();
				return;
			}

			log(trivial, "TcpConnection") << "async send data, len: " << len;
		});
}

void TcpConnection::Disconnect() {
	if (_state == Connected) {
		_sock.cancel(); // mark
		// _sock.shutdown(tcp::socket::shutdown_both);
		_sock.close();
		_closeCallback(shared_from_this());
		log(debug, "TcpConnection") << "socket close.";

	}

	_state = Disconnected;
}

/// TcpService
TcpService::TcpService(std::string name) :
	_name(name),
	_acceptor(_io_context),
	_heartTimer(_io_context),
	_messageCallback(std::bind(&DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)),
	_connectionCallback(std::bind(&DefaultConnectionCallback, std::placeholders::_1)){

}

void TcpService::AsyncListen(unsigned int port) {
	if (port == 0) {
		log(error, "TcpService") << "acceptor listen failure, error message: port = 0.";
		return;
	}

	tcp::endpoint local(tcp::v4(), port);
	_acceptor.open(local.protocol());

	error_code code;

	_acceptor.bind(local, code);
	if (code) {
		log(error, "TcpService") << "acceptor bind port: " << port << " failure, error message: " << code.message();
		return;
	}

	_acceptor.listen(asio::socket_base::max_listen_connections, code);
	if (code) {
		log(error, "TcpService") << "acceptor listen port " << port << " failure, error code: " << code.message();
		return;
	}

	AsyncListenInLoop();

	log(normal, "TcpService") << "acceptor listening... port: " << port;
}


TcpConnectionPtr TcpService::AsyncConnect(std::string ip, unsigned int port) {
	tcp::endpoint endpoint = tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
	TcpConnectionPtr pConnection = std::make_shared<TcpConnection>(std::move(tcp::socket(_io_context)));
	if (pConnection) {
		pConnection->GetSocket().async_connect(endpoint,
			[this, pConnection](const error_code& code) {
				if (code) {
					log(error, "TcpService") << "connect failure, error message: " << code.message();
					return;
				}

				pConnection->SetMessageCallback(_messageCallback);
				pConnection->SetCloseCallback(std::bind(&TcpService::RemoveActiveConnection, this, std::placeholders::_1));
				pConnection->OnEstablish();

				_activeConnections.insert({ pConnection->GetID(), pConnection });

				_connectionCallback(pConnection);
			}
		);
	}

	return pConnection;
}

void TcpService::RemoveActiveConnection(const TcpConnectionPtr& pConnection) {
	auto it = _activeConnections.find(pConnection->GetID());
	if (it != _activeConnections.end()) {
		assert(pConnection == it->second);

		_activeConnections.erase(pConnection->GetID());
	}
}
void TcpService::RemovePassiveConnection(const TcpConnectionPtr& pConnection) {
	auto it = _passiveConnections.find(pConnection->GetID());
	if (it != _passiveConnections.end()) {
		assert(pConnection == it->second);

		_passiveConnections.erase(pConnection->GetID());
	}
}
void TcpService::AsyncListenInLoop() {
	TcpConnectionPtr pConnection = std::make_shared<TcpConnection>(std::move(tcp::socket(_io_context)));
	_acceptor.async_accept(pConnection->GetSocket(), [this, pConnection](const error_code& code) { // pSock: capture by value, keep alive
		if (code) {
			log(error, "TcpService") << "async accept failure, error message: " << code.message();
			return;
		}

		pConnection->SetMessageCallback(_messageCallback);
		pConnection->SetCloseCallback(std::bind(&TcpService::RemovePassiveConnection, this, std::placeholders::_1));
		pConnection->OnEstablish();

		_passiveConnections.insert({ pConnection->GetID(), pConnection });

		_connectionCallback(pConnection);

		AsyncListenInLoop();
		}
	);
}

void TcpService::Start() {
	_io_thread = std::thread([this] {_io_context.run(); });
}

void TcpService::Close() {
	for (auto it = _activeConnections.begin(); it != _activeConnections.end();) {
		const TcpConnectionPtr& pConnection = it->second;
		if (pConnection && pConnection->IsConnectioned()) {
			pConnection->Disconnect();
		}
		else {
			++it;
		}
	}

	for (auto it = _passiveConnections.begin(); it != _passiveConnections.end();) {
		const TcpConnectionPtr& pConnection = it->second;
		if (pConnection && pConnection->IsConnectioned()) {
			pConnection->Disconnect();
		}
		else {
			++it;
		}
	}
}