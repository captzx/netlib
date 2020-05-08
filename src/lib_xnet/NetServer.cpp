#include "NetServer.h"

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

using namespace std::placeholders;

int connection_count = 0;

NetServer::NetServer(std::string name):
	_name(name),
	_acceptor(_io_context) {
	// _messageCallback(std::bind(&NetServer::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2));
}

void NetServer::Listen(unsigned int port) {
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

//void NetServer::DefaultMessageCallback(const TcpConnectionPtr&, Buffer&) {
//	log(debug) << "tcp connection default message call back.";
//}

void NetServer::AsyncListenInLoop() {
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
			AsyncListenInLoop();
		});
}
