//
// blocking_tcp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2019 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "common.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace piece::tool;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");

	try
	{
		io_context io_context;

		tcp::socket sock(io_context);
		sock.connect(tcp::endpoint(ip::address::from_string("127.0.0.1"), 1234));
		log(debug) << "local: " << sock.local_endpoint() << ", remote: " << sock.remote_endpoint()<< "... connect success" << std::endl;

		std::cout << "Enter message: ";
		char request[max_length];
		std::cin.getline(request, max_length);
		size_t request_length = std::strlen(request);
		sock.send(buffer(request, request_length));

		char reply[max_length];
		size_t reply_length = sock.receive(buffer(reply, request_length));
		std::cout << "Reply is: ";
		std::cout.write(reply, reply_length);
		std::cout << "\n";
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	// global_logger_stop2(sink);

	return 0;
}
