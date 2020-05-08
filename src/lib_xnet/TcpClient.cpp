#include "TcpClient.h"

#include <xtools/Logger.h>

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

TcpClient::TcpClient(std::string name) :
	_name(name) {

}

void TcpClient::Connect(tcp::endpoint end) {
	SocketPtr pSock = std::make_shared<tcp::socket>(_io_context);
	
	error_code code;
	pSock->connect(end, code);
	if (code) {
		log(error) << "connect failure, error message: " << code.message();
		return;
	}

	if (pSock->is_open()) {
		_pConnection = std::make_shared<TcpConnection>(std::move(pSock));
		_pConnection->Established();
		log(error) << "tcp client connect success.";

		std::thread([&] { _io_context.run(); }).detach();
	}
}
