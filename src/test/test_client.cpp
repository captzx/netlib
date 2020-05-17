#include <xtools/Common.h>
#include <xnet/ProtobufProcess.h>
#include <xnet/Buffer.h>
#include <xnet/TcpService.h>

#include <xtools/Crypto.h>

#include <xprotos/login.pb.h>

using namespace x::tool;
using namespace x::net;

enum { max_length = 1024 };

class TestClient {
public:
	explicit TestClient(IOContext& ctx, std::string name) :
		_dispatcher(std::bind(&TestClient::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
		_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

		_pTcpClient = std::make_shared<TcpClient>(ctx, name);
		_pTcpClient->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
		_pTcpClient->SetConnectionCallback(std::bind(&TestClient::OnConnection, this, std::placeholders::_1));

		_dispatcher.RegisterMessageCallback<HeartBeat>(std::bind(&TestClient::OnHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
		_dispatcher.RegisterMessageCallback<ResponseRsaPublicKey>(std::bind(&TestClient::OnResponseRsaPublicKey, this, std::placeholders::_1, std::placeholders::_2));
	}

public:
	void AsyncConnect(std::string ip, unsigned int port) {
		if(_pTcpClient) _pTcpClient->AsyncConnect(ip, port);
	}
	bool IsConnectioned() {
		if (_pTcpClient) return _pTcpClient->IsConnectioned();

		return false;
	}
	void Close() {
		if (_pTcpClient) {
			_pTcpClient->Close();
		}
	}
	void AsyncSend(Buffer& buf) {
		if (_pTcpClient) _pTcpClient->AsyncSend(buf);
	}

public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
		log(debug, "TestClient") << "client default message call back.";
	}
	void OnConnection(const TcpConnectionPtr&) {
		log(debug, "TestClient") << "client onConnection.";
	}
	void OnHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<HeartBeat>& pMessage) {
		unsigned int now = GetSystemTime();
		int diff = pMessage->time() - now;

		log(normal, "TestClient") << "recv heart beat: " << now << ", diff: " << diff;

		pConnection->SetLastHeartBeatTime(now);

		pMessage->set_time(now);

		Buffer buf;
		ProtobufCodec::PackMessage(pMessage, buf);
		pConnection->AsyncSend(buf);
	}
	void OnResponseRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<ResponseRsaPublicKey>& pMessage) {
		_public_key = pMessage->publickey();

		std::cout << "_public_key: " << _public_key << std::endl;
	}

	const std::string& PublicKey() {
		return _public_key;
	}
private:
	TcpClientPtr _pTcpClient;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;
	std::string _public_key;
};

int main(int argc, char* argv[])
{
	global_logger_init("./log/client.log");
	global_logger_set_filter(severity >= debug);

	try
	{
		IOContext ctx;
		TestClient client(ctx, "TestClient");
		client.AsyncConnect("127.0.0.1", 1234);
		ctx.RunInThread();

		char line[20];
		while (client.IsConnectioned() && std::cin.getline(line, 20)) {
			if (client.PublicKey().empty()) {
				auto pMsg = std::make_shared<RequestRsaPublicKey>();
				if (pMsg) {
					Buffer buf;
					ProtobufCodec::PackMessage(pMsg, buf);
					client.AsyncSend(buf);
				}
			}
			else {
				auto pMsg = std::make_shared<RequestRegister>();
				if (pMsg) {
					pMsg->set_account("captzx");
					pMsg->set_password(RSAEncrypt(client.PublicKey(), "123"));
					// pMsg->set_password("123");

					
					Buffer buf;
					ProtobufCodec::PackMessage(pMsg, buf);
					client.AsyncSend(buf);
				}
			}
		}

		client.Close();
		ctx.Stop();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}