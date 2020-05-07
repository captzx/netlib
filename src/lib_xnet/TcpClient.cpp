#include "TcpClient.h"

#include <xtools/Logger.h>

using namespace boost;
using namespace x::net;
using namespace x::tool;

using boost::asio::ip::tcp;
using boost::system::error_code;

int connection_count = 0;

TcpClient::TcpClient(std::string name) :
	_name(name) {

}

void TcpClient::Connect(tcp::endpoint end) {
	SocketPtr pSock = std::make_shared<tcp::socket>(_io_context);
	pSock->async_connect(end, [this](const error_code& code)
		{
			if (code) {
				log(error) << "async connect failure, error message: " << code.message();
				return;
			}

			std::make_shared<TcpConnection>(std::move(pSock))->Established();
		});
}
