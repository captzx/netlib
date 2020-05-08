#pragma once 

#include "Using.h"
#include "TcpConnection.h"

namespace x {

namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_context;

class TcpClient {
public:
	TcpClient(std::string);

public:
	void Connect(tcp::endpoint ep);
	void Close() {
		if (_pConnection) _pConnection->Close();
	}
	void AsyncSend(Buffer& buf) {
		if(_pConnection) _pConnection->AsyncSend(buf);
	}

private:
	std::string _name;
	io_context _io_context;
	TcpConnectionPtr _pConnection;
};

using TcpClientPtr = std::shared_ptr<TcpClient>;
} // namespace net

} // namespace x