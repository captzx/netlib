#include "NetServer.h"

#include <xtools/Logger.h>

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

int connection_count = 0;

NetServer::NetServer(std::string name):
	_name(name),
	_acceptor(_io_context) {

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


void NetServer::AsyncListenInLoop() {
	SocketPtr pSock  = std::make_shared<tcp::socket>(_io_context);
	_acceptor.async_accept(*pSock,
		[this, pSock](const error_code& code)
		{
			if (code) {
				log(error) << "async accept failure, error message: " << code.message();
				return;
			}

			/*std::shared_ptr<TcpConnection> pConnection = std::make_shared<TcpConnection>(pSock);
				_tcpConnections.insert({connection_count++, pConnection});
				pConnection->Established();*/
			std::make_shared<TcpConnection>(std::move(pSock))->Established();
			AsyncListenInLoop();
		});
}
