#include "common.h"

#include <xprotos/login.pb.h>
#include <xnet/ProtobufCodec.h>
#include <xnet/Buffer.h>

#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace x::tool;
using namespace x::net;

enum { max_length = 1024 };

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");

	try
	{
		io_context io_context;

		tcp::socket sock(io_context);
		sock.connect(tcp::endpoint(ip::address::from_string("127.0.0.1"), 1234));
		log(debug) << "local: " << sock.local_endpoint() << ", remote: " << sock.remote_endpoint() << "... connect success" << std::endl;

		SearchRequest msg;
		msg.set_query("captzx");
		msg.set_page_number(1);
		msg.set_result_per_page(1);

		Buffer buf;
		ProtobufCodec::PackMessage(buf, msg);

		std::cout << "Enter message: len = " << buf.Readable() << std::endl;
		
		sock.send(buffer(buf.ReadPtr(), buf.Readable()));

		/*char reply[max_length];
		size_t reply_length = sock.receive(buffer(reply, request_length));
		std::cout << "Reply is: ";
		std::cout.write(reply, reply_length);
		std::cout << "\n";*/
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	// global_logger_stop2(sink);

	return 0;
}