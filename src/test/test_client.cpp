#include <xtools/Common.h>
#include <xprotos/login.pb.h>
#include <xnet/ProtobufCodec.h>
#include <xnet/Buffer.h>
#include <xnet/TcpClient.h>
#include <xnet/TcpConnection.h>

#include <boost/asio.hpp>

using namespace boost::asio;
using boost::asio::ip::tcp;

using namespace x::tool;
using namespace x::net;

enum { max_length = 1024 };

class TestClient {
public:
	explicit TestClient(std::string name) {
		_pTcpClient = std::make_shared<TcpClient>(name);
	}

public:
	void Connect(std::string ip, unsigned int port) {
		if(_pTcpClient) _pTcpClient->Connect(tcp::endpoint(ip::address::from_string(ip), port));
	}

	void Close() {
		if (_pTcpClient) _pTcpClient->Close();
	}
	void AsyncSend(Buffer& buf) {
		if (_pTcpClient) _pTcpClient->AsyncSend(buf);
	}
private:
	TcpClientPtr _pTcpClient;
};

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");

	try
	{
		TestClient client("TestClient");
		client.Connect("127.0.0.1", 1234);

		{
			SearchRequest msg;
			msg.set_query("captzx");
			msg.set_page_number(1);
			msg.set_result_per_page(1);

			Buffer buf;
			ProtobufCodec::PackMessage(buf, msg);
			using namespace std::chrono_literals;
			std::cout << "Enter message: len = " << buf.Readable() << std::endl;

			// client.Send(buffer(buf.ReadPtr(), buf.Readable()));
			client.AsyncSend(buf);
			std::this_thread::sleep_for(1s);
		}
		{
			SearchResponse msg;
			msg.set_result(1);

			Buffer buf;
			ProtobufCodec::PackMessage(buf, msg);
			using namespace std::chrono_literals;
			std::cout << "Enter message: len = " << buf.Readable() << std::endl;

			client.AsyncSend(buf);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	// global_logger_stop2(sink);

	return 0;
}