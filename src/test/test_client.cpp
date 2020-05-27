#include <xtools/Common.h>

#include "UseTools.h"
#include "UseNet.h"

#include <xprotos/Server.pb.h>
#include <xprotos/Login.pb.h>
#include <xprotos/Scene.pb.h>

using namespace x::tool;
using namespace x::net;

class TestClient {
public:
	explicit TestClient(std::string name) :
		_dispatcher(std::bind(&TestClient::DefaultMessageCallback, this, _1, _2)),
		_codec(std::bind(&ProtobufDispatcher::RecvMessage, &_dispatcher, _1, _2)) {

		_pTcpService = std::make_shared<TcpService>(name);

		_dispatcher.RegisterMessageCallback<ActiveHeartBeat>(std::bind(&TestClient::OnActiveHeartBeat, this, _1, _2));
		_dispatcher.RegisterMessageCallback<RspRsaPublicKey>(std::bind(&TestClient::OnRspRsaPublicKey, this, _1, _2));
		_dispatcher.RegisterMessageCallback<RegisterResult>(std::bind(&TestClient::OnRegisterResult, this, _1, _2));
		_dispatcher.RegisterMessageCallback<LoginResult>(std::bind(&TestClient::OnLoginResult, this, _1, _2));
		_dispatcher.RegisterMessageCallback<ReturnZoneList>(std::bind(&TestClient::OnReturnZoneList, this, _1, _2));
		_dispatcher.RegisterMessageCallback<RspZoneRoleData>(std::bind(&TestClient::OnRspZoneRoleData, this, _1, _2));
		_dispatcher.RegisterMessageCallback<RspCreateRole>(std::bind(&TestClient::OnRspCreateRole, this, _1, _2));
		
		
		Init();
	}

	struct ZoneServer {
		int32_t id = 0;
		std::string name;
		std::string ip;
		uint32_t port = 0;
		uint32_t state = 0;
	};

public:
	void Init() {
		_state = 0;

		global_logger_init("./log/client.log");
		global_logger_set_filter(severity >= debug);

		_pTcpService->SetMessageCallback(std::bind(&ProtobufCodec::RecvMessage, &_codec, _1, _2));
		_pTcpService->SetAtvConnCallback(std::bind(&TestClient::OnAtvConnection, this, _1));
	}

	void Start() { 
		_pConnection = _pTcpService->AsyncConnect("127.0.0.1", 1235);

		_pTcpService->Start();
		_state = 1;
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

				if (line[0] == '-') {
					log(normal, "TestClient") << "check state: " << _state;
					continue;
				}

				if (_state == 1) {
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
						auto pMsg = std::make_shared<ReqRsaPublicKey>();
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

						auto pMsg = std::make_shared<ReqRegister>();
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

						auto pMsg = std::make_shared<ReqLogin>();
						if (pMsg) {
							pMsg->set_account("captzx");
							pMsg->set_password(RSAEncrypt(_key, "123"));
							_pConnection->AsyncSend(pMsg);
						}

						log(normal, "TestClient") << "request login!";
					}
					break;
					case '4':
					{
						auto pMsg = std::make_shared<SelectZoneServer>();
						if (pMsg) {
							pMsg->set_act_id(_act_id);
							pMsg->set_zone_id(1); // zone_id

							_pConnection->AsyncSend(pMsg);
						}

						log(normal, "TestClient") << "select zone server 1!";
					}
					break;
					case '5':
					{
						auto pMsg = std::make_shared<ReqCreateRole>();
						if (pMsg) {
							pMsg->set_act_id(_act_id);
							pMsg->set_zone_id(1); // zone_id
							pMsg->set_name("captzx");

							_pConnection->AsyncSend(pMsg);
						}
					}
					break;
					case '6':
					{
						auto pMsg = std::make_shared<ReqEnterGame>();
						if (pMsg) {
							pMsg->set_act_id(_act_id);
							pMsg->set_role_id(2147549187);

							_pConnection->AsyncSend(pMsg);

							_pConnection = _pTcpService->AsyncConnect(_ip, _port);
							_state = 2;
						}
					}
					default:
						break;
					}
				}
				else if (_state == 2) {
					auto pMsg = std::make_shared<OffLine>();
					if (pMsg) _pConnection->AsyncSend(pMsg);
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
	void OnAtvConnection(const TcpConnectionPtr& pConnection) {
		assert(_pConnection == pConnection);

		log(debug, "TestClient") << "client onConnection, start op.";
	}
	void OnActiveHeartBeat(const TcpConnectionPtr& pConnection, const std::shared_ptr<ActiveHeartBeat>& pMessage) {
		uint32_t now = Now::Second();
		int32_t diff = pMessage->last_hb_time() - now;

		log(trivial, "TestClient") << "recv heart beat: " << now << ", diff: " << diff;

		pConnection->SetLastHeartBeatTime(now);

		auto pMsg = std::make_shared<PassiveHeartBeat>();
		if (pMsg) {
			pMsg->set_last_hb_time(pConnection->GetLastHeartBeatTime());
			pMsg->set_cnt_start_time(pConnection->GetStartTime());
			pMsg->set_address(pConnection->GetLocalEndpoint());
			pMsg->set_pid(pConnection->GetPid());
			pMsg->set_atv_count(_pTcpService->GetAtvConnMgr().GetConnectedCount());
			pMsg->set_psv_count(_pTcpService->GetPsvConnMgr().GetConnectedCount());

			pConnection->AsyncSend(pMsg);
		}
	}
	void OnRspRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspRsaPublicKey>& pMessage) {
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
			_act_id = pMessage->act_id();
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
	void OnReturnZoneList(const TcpConnectionPtr& pConnection, const std::shared_ptr<ReturnZoneList>& pMsg) {
		for (int32_t i = 0; i < pMsg->zone_servers_size(); ++i) {
			auto zone = pMsg->zone_servers(i);

			uint32_t zoneid = zone.id();

			_zoneList[zoneid].id = zone.id();
			_zoneList[zoneid].name = zone.name();
			_zoneList[zoneid].ip = zone.ip();
			_zoneList[zoneid].port = zone.port();

			log(debug, "TestClient") << "recv zone server: " << zone.id() << " " << zone.name() << " " << zone.ip() << " " << zone.port();
		}
	}
	void OnRspZoneRoleData(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspZoneRoleData>& pMsg) {
		log(debug, "TestClient") << "recv RoleList";

		RoleList roleList = pMsg->rolelist();

		for (int32_t i = 0; i < roleList.roles_size(); ++i) {
			auto role = roleList.roles(i);

			log(debug, "TestClient") << role.name() << " " << role.role_id();
		}

		_ip = pMsg->ip();
		_port = pMsg->port();
	}
	void OnRspCreateRole(const TcpConnectionPtr& pConnection, const std::shared_ptr<RspCreateRole>& pMsg) {
		RoleInfo role = pMsg->role();
		log(debug, "TestClient") << "recv OnRspCreateRole, roleid = " << role.role_id() << ", name = " << role.name();
	}
	

private:
	TcpServicePtr _pTcpService;
	ProtobufDispatcher _dispatcher;
	ProtobufCodec _codec;

	TcpConnectionPtr _pConnection;

	std::string _key;
	uint64_t _act_id;
	std::map<uint32_t, ZoneServer> _zoneList;
	std::string _ip;
	uint32_t _port;
	uint32_t _state;
};

int32_t main(int32_t argc, char* argv[])
{
	TestClient testClient("TestClient");
	testClient.Start();
	testClient.Test();

	return 0;
}