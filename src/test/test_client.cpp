#include <xtools/Common.h>
#include <xnet/ProtobufProcess.h>
#include <xnet/Buffer.h>
#include <xnet/TcpService.h>

#include <xprotos/login.pb.h>

using namespace x::tool;
using namespace x::net;

enum { max_length = 1024 };

class TestClient {
public:
	explicit TestClient(std::string name) :
		_dispatcher(std::bind(&TestClient::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
		_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

		_pTcpClient = std::make_shared<TcpClient>(name);
		_pTcpClient->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
		_pTcpClient->SetConnectionCallback(std::bind(&TestClient::OnConnection, this, std::placeholders::_1));

		_dispatcher.RegisterMessageCallback(SearchRequest::descriptor(), std::bind(&TestClient::OnSearchRequest, this, std::placeholders::_1, std::placeholders::_2));
		_dispatcher.RegisterMessageCallback(SearchResponse::descriptor(), std::bind(&TestClient::OnSearchResponse, this, std::placeholders::_1, std::placeholders::_2));
	}

public:
	void AsyncConnect(std::string ip, unsigned int port) {
		if(_pTcpClient) _pTcpClient->AsyncConnect(ip, port);
	}

	void Close() {
		if (_pTcpClient) _pTcpClient->Close();
	}
	void AsyncSend(Buffer& buf) {
		if (_pTcpClient) _pTcpClient->AsyncSend(buf);
	}

public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
		log(debug) << "client default message call back.";
	}
	void OnSearchRequest(const TcpConnectionPtr&, const MessagePtr&) {
		log(debug) << "client onSearchRequest.";
	}
	void OnSearchResponse(const TcpConnectionPtr&, const MessagePtr&) {
		log(debug) << "client onSearchResponse.";
	}
	void OnConnection(const TcpConnectionPtr&) {
		log(debug) << "client onConnection.";
	}

private:
	TcpClientPtr _pTcpClient;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
};

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");

	try
	{
		TestClient client("TestClient");
		client.AsyncConnect("127.0.0.1", 1234);

		while (1) {

			{
				auto pMsg = std::make_shared<SearchRequest>();
				if (pMsg) {
					pMsg->set_query("captzx");
					pMsg->set_page_number(1);
					pMsg->set_result_per_page(1);
				}

				Buffer buf;
				ProtobufCodec::PackMessage(pMsg, buf);
				using namespace std::chrono_literals;
				std::cout << "Enter message: len = " << buf.Readable() << std::endl;

				// client.Send(buffer(buf.ReadPtr(), buf.Readable()));
				client.AsyncSend(buf);
				std::this_thread::sleep_for(1s);
			}
			{
				auto pMsg = std::make_shared<SearchResponse>();
				if (pMsg) {
					pMsg->set_result(1);
				}

				Buffer buf;
				ProtobufCodec::PackMessage(pMsg, buf);
				using namespace std::chrono_literals;
				std::cout << "Enter message: len = " << buf.Readable() << std::endl;

				client.AsyncSend(buf);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	// global_logger_stop2(sink);

	return 0;
}