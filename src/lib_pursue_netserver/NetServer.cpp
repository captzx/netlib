#include "common.h"

#include "NetServer.h"
#include "TcpConnection.h"

using boost::asio::ip::tcp;
using boost::system::error_code;
using namespace boost;
using namespace piece::net;
using namespace piece::tool;

NetServer::NetServer():
	_acceptor(_io_context) {

}

void NetServer::Init() {
	log(normal) << "net server initialize ... success!";
}
void NetServer::Listen(unsigned int port) {
	if (port == 0) {
		log(error) << "listen failure... port: " << port;
		return;
	}
	tcp::endpoint local_ep(tcp::v4(), port);
	_acceptor.open(local_ep.protocol());
	error_code ec;
	_acceptor.bind(local_ep, ec);
	if (ec) {
		log(error) << "bind port: " << port << "... failure, error code: " << ec;
		return;
	}

	_acceptor.listen(asio::socket_base::max_listen_connections, ec);
	if (ec)
	{
		log(error) << "listening port: " << port << "... failure, error code: " << ec;
		return;
	}

	log(normal) << "listening... port: " << port;

	_Listening();
	_io_context.run();
}


void NetServer::_Listening() {
	std::shared_ptr<tcp::socket> pSock  = std::make_shared<tcp::socket>(_io_context);
	_acceptor.async_accept(*pSock,
		[&](const error_code& ec)
		{
			if (!ec)
			{
				std::make_shared<TcpConnection>(pSock)->Established();
			}

			_Listening();
		});
}
