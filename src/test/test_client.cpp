#include <xtools/Common.h>

#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>

using namespace x::tool;
using namespace x::net;

class TestClient {
public:
	explicit TestClient(std::string name) :
		_dispatcher(std::bind(&TestClient::DefaultMessageCallback, this, std::placeholders::_1, std::placeholders::_2)),
		_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, std::placeholders::_1, std::placeholders::_2)) {

		_pTcpService = std::make_shared<TcpService>(name);

		_dispatcher.RegisterMessageCallback<HeartBeat>(std::bind(&TestClient::OnHeartBeat, this, std::placeholders::_1, std::placeholders::_2));
		_dispatcher.RegisterMessageCallback<ResponseRsaPublicKey>(std::bind(&TestClient::OnResponseRsaPublicKey, this, std::placeholders::_1, std::placeholders::_2));
		_dispatcher.RegisterMessageCallback<RegisterResult>(std::bind(&TestClient::OnRegisterResult, this, std::placeholders::_1, std::placeholders::_2));
		_dispatcher.RegisterMessageCallback<LoginResult>(std::bind(&TestClient::OnLoginResult, this, std::placeholders::_1, std::placeholders::_2));

		Init();
	}

public:
	void Init() {
		global_logger_init("./log/client.log");
		global_logger_set_filter(severity >= trivial);

		_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, std::placeholders::_1, std::placeholders::_2));
		_pTcpService->SetConnectionCallback(std::bind(&TestClient::OnConnection, this, std::placeholders::_1));
	}

	void Start() { 
		_pConnection = _pTcpService->AsyncConnect("127.0.0.1", 1232);

		_pTcpService->Start();
	}

	void Close() { _pTcpService->Close(); }

	void Test() {
		try
		{
			char line[20];
			while (std::cin.getline(line, 20)) {
				if (!_pConnection->IsConnectioned()) {
					log(normal, "TestClient") << "not connection!";
					continue;
				}
				switch (line[0])
				{
				case '0':
				{
					Close();
					log(normal, "TestClient") << "active close!";
				}
				break;
				case '1':
				{
					auto pMsg = std::make_shared<RequestRsaPublicKey>();
					_pConnection->AsyncSend(pMsg);
					log(normal, "TestClient") << "request ras public key!";
				}
				break;
				case '2':
				{
					if (_key.empty()) {
						log(normal, "TestClient") << "request ras public key first, please!";
						break;
					}

					auto pMsg = std::make_shared<RequestRegister>();
					if (pMsg) {
						pMsg->set_account("captzx");
						pMsg->set_password(RSAEncrypt(_key, "123"));
						_pConnection->AsyncSend(pMsg);
					}

					log(normal, "TestClient") << "request register!";
				}
				break;
				case '3':
				{
					if (_key.empty()) {
						log(normal, "TestClient") << "request ras public key first, please!";
						break;
					}

					auto pMsg = std::make_shared<RequestLogin>();
					if (pMsg) {
						pMsg->set_account("captzx");
						pMsg->set_password(RSAEncrypt(_key, "1234"));
						_pConnection->AsyncSend(pMsg);
					}

					log(normal, "TestClient") << "request login!";
				}
				break;
				default:
					break;
				}
			}
		}
		catch (std::exception& e)
		{
			std::cerr << "Exception: " << e.what() << "\n";
		}
	}

public:
	void DefaultMessageCallback(const TcpConnectionPtr&, const MessagePtr&) {
		log(debug, "TestClient") << "client default message call back.";
	}
	void OnConnection(const TcpConnectionPtr& pConnection) {
		assert(_pConnection == pConnection);

		log(debug, "TestClient") << "client onConnection, start op.";
	}
	void OnHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<HeartBeat>& pMessage) {
		unsigned int now = Now::Second();
		int diff = pMessage->time() - now;

		log(trivial, "TestClient") << "recv heart beat: " << now << ", diff: " << diff;

		pConnection->SetLastHeartBeatTime(now);

		pMessage->set_time(now);
		pConnection->AsyncSend(pMessage);
	}
	void OnResponseRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<ResponseRsaPublicKey>& pMessage) {
		_key = pMessage->publickey();
	}
	void OnRegisterResult(const TcpConnectionPtr& pConnection, const std::shared_ptr<RegisterResult>& pMessage) {
		switch (pMessage->result())
		{
		case 0:
			log(warning, "TestClient") << "todo: login success!";
			break;
		case 1:
			log(warning, "TestClient") << "todo: already register!";
			break;
		default:
			break;
		}
	}	
	void OnLoginResult(const TcpConnectionPtr& pConnection, const std::shared_ptr<LoginResult>& pMessage) {
		switch (pMessage->result())
		{
		case 0:
			log(warning, "TestClient") << "todo: login success!";
			break;
		case 1:
			log(warning, "TestClient") << "todo: not register!";
			break;
		case 2:
			log(warning, "TestClient") << "todo: error password!";
			break;
		default:
			break;
		}
	}

private:
	TcpServicePtr _pTcpService;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;

	TcpConnectionPtr _pConnection;

	std::string _key;
};

int main(int argc, char* argv[])
{
	TestClient testClient("TestClient");
	testClient.Start();
	testClient.Test();

	return 0;
}