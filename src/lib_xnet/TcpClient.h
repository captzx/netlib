#pragma once 

#include <boost/asio.hpp>
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
	private:
		std::string _name;
		io_context _io_context;
	};

} // namespace net

} // namespace x