#include "common.h"

#include <xprotos/login.pb.h>
#include <xnet/ProtobufCodec.h>
#include <xnet/Buffer.h>
#include <xnet/TcpClient.h>

#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace x::tool;
using namespace x::net;

enum { max_length = 1024 };

using x::net::NetServer;
using TcpClientPtr = std::shared_ptr<TcpClient>;
class TestClient {
public:
	explicit TestClient(std::string);

public:
	void Connect(std::string ip, unsigned int port) {
		_pTcpClient.Connect(tcp::endpoint(ip::address::from_string(ip), port));
	}

	void Clost() {

	}
	void Send() {

	}
private:
	TcpClientPtr _pTcpClient;
};

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");

	try
	{
		TestClient client;
		client.Connect("127.0.0.1", 1234);

		int i = 0;
		while (i++ != 100) {

			{
				SearchRequest msg;
				msg.set_query("captzx");
				msg.set_page_number(1);
				msg.set_result_per_page(1);

				Buffer buf;
				ProtobufCodec::PackMessage(buf, msg);
				using namespace std::chrono_literals;
				std::cout << "Enter message: len = " << buf.Readable() << std::endl;

				client.Send(buffer(buf.ReadPtr(), buf.Readable()));
				std::this_thread::sleep_for(100ms);
			}
			{
				SearchResponse msg;
				msg.set_result(1);

				Buffer buf;
				ProtobufCodec::PackMessage(buf, msg);
				using namespace std::chrono_literals;
				std::cout << "Enter message: len = " << buf.Readable() << std::endl;

				client.Send(buffer(buf.ReadPtr(), buf.Readable()));
			}

		}

		client.Close();

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