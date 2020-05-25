#include "LoginUserManager.h"

#include "LoginUser.h"
#include "LoginServer.h"
#include "ZoneServerManager.h"

using namespace mysqlx;
using namespace x::login;
using namespace x::tool;

LoginUserManager::LoginUserManager() {
	
}

void LoginUserManager::Init() {
	x::tool::GenerateRSAKey(_private_key, _public_key);
	_uuidGenerator.Init(0, 0);
}

void LoginUserManager::RegisterMessageCallback() {
	ProtobufDispatcher& dispatcher = LoginServer::GetInstance().GetDispatcher();

	dispatcher.RegisterMessageCallback<RequestRsaPublicKey>(std::bind(&LoginUserManager::OnRequestRsaPublicKey, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RequestRegister>(std::bind(&LoginUserManager::OnRequestRegister, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RequestLogin>(std::bind(&LoginUserManager::OnRequestLogin, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginUserManager::OnRequestRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRsaPublicKey>&) {
	auto pMsg = std::make_shared<ResponseRsaPublicKey>();
	if (pMsg) {
		pMsg->set_publickey(_public_key);

		pConnection->AsyncSend(pMsg);
	}
}

void LoginUserManager::OnRequestRegister(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRegister>& pMsg) {
	std::string account = pMsg->account();
	std::string password = RSADecrypt(_private_key, pMsg->password());

	log(debug, "LoginUserManager") << "register account: " << account << ", password: " << password;

	if (VerifyAccount(account)) {
		std::shared_ptr<LoginUser> pUser = CreateUser(account);
		if (SaveUser(pUser, account, password)) {
			SendRegisterResult(pConnection, 0); // success
			_userManager.insert({ pUser->GetID(), pUser });

			log(debug, "LoginUserManager") << "create user, user id: " << pUser->GetID();
			log(debug, "LoginUserManager") << "user manager size: " << _userManager.size();
		}
	}
	else SendRegisterResult(pConnection, 1); // alread existed
}

bool LoginUserManager::SendRegisterResult(const TcpConnectionPtr& pConnection, int result) {
	auto pMsg = std::make_shared<RegisterResult>();
	if (pMsg) {
		pMsg->set_result(result);

		pConnection->AsyncSend(pMsg);
	}

	return true;
}


void LoginUserManager::OnRequestLogin(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestLogin>& pMsg) {
	std::string account = pMsg->account();
	std::string password = RSADecrypt(_private_key, pMsg->password());
	std::string inputPassword = PBKDFEncrypt(password, SHA3_256Hash(password));

	std::ostringstream oss;

	oss << "SELECT user.`id`, user.`pwd` FROM x_login.user WHERE user.`act` = '" << account << "';";

	auto pSend = std::make_shared<LoginResult>();

	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_login)->ExecuteSql(oss.str());
	if (result.count() == 0) pSend->set_result(1);
	else {
		assert(result.count() == 1);
		Row row = result.fetchOne();
		assert(!row.isNull());
		ull userid = static_cast<uint64_t>(row.get(0));
		std::string existPassword = static_cast<std::string>(row.get(1));
		if (inputPassword == existPassword) {
			std::shared_ptr<LoginUser> pUser = std::make_shared<LoginUser>();
			pUser->SetID(userid);
			pUser->SetState(LoginState::ONLINE);
			pUser->SetLastAccessTime(Now::Second());
			pUser->SetConnection(pConnection);
			_userManager[userid] = pUser;

			pSend->set_result(0);
			pSend->set_act_id(userid);

			ZoneServerManager::GetInstance().SendAllZoneList(pConnection);
		}
		else {
			pSend->set_result(2);
		}
	}

	pConnection->AsyncSend(pSend);
}

bool LoginUserManager::VerifyAccount(const std::string& account) {
	std::ostringstream oss;

	oss << "SELECT user.`act` FROM x_login.user WHERE user.`act` = '" << account << "';";
	
	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((unsigned int)DBType::x_login)->ExecuteSql(oss.str());

	if (result.count() != 0) {
		log(debug, "LoginUserManager") << "account existed, login or pick another!";
		return false;
	}

	return true;
}

bool LoginUserManager::VerifyPassword(const std::string& account, const std::string& password) {
	log(debug, "LoginUserManager") << "todo: verify password";
	return true;
}

std::shared_ptr<LoginUser> LoginUserManager::CreateUser(const std::string& account) {
	std::shared_ptr<LoginUser> pUser = std::make_shared<LoginUser>();
	pUser->SetID(_uuidGenerator.Get());
	pUser->SetState(LoginState::OFFLINE);
	pUser->SetLastAccessTime(Now::Second());

	return pUser;
}

bool LoginUserManager::SaveUser(const std::shared_ptr<LoginUser>& pUser, const std::string& account, const std::string& password) {
	std::string encrypt = PBKDFEncrypt(password, SHA3_256Hash(password)); // fix, don't use password generate salt

	std::ostringstream oss;

	oss << "INSERT INTO x_login.user(`id`, `act`, `pwd`) VALUES (" << pUser->GetID() << ", '" << account << "', '" << encrypt << "');";

	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	pDBService->GetDBConnectionByType((unsigned int)DBType::x_login)->ExecuteSql(oss.str());

	return true;
}
