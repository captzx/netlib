#include "LoginUserManager.h"

#include "LoginServer.h"
#include "ZoneServerManager.h"

using namespace x::login;

/// LoginUser
LoginUser::LoginUser() :
	_id(0),
	_state(LoginState::OFFLINE),
	_lastAccessTime(0) {

}

/// LoginUserManager
LoginUserManager::LoginUserManager() {
	
}

void LoginUserManager::Init() {
	x::tool::GenerateRSAKey(_private_key, _public_key);
	_uuidGenerator.Init(0, 0);
}

std::shared_ptr<LoginUser> LoginUserManager::FindUser(uint64_t actid) {
	auto it = _userManager.find(actid);
	if (it != _userManager.end()) {
		return it->second;
	}

	return nullptr;
}

void LoginUserManager::RegisterMessageCallback() {
	GET_MESSAGE_DISPATCHER(LoginServer::GetInstance().GetDispatcher());
	REGISTER_MESSAGE_CALL_BACK(LoginUserManager, this, ReqRsaPublicKey);
	REGISTER_MESSAGE_CALL_BACK(LoginUserManager, this, ReqRegister);
	REGISTER_MESSAGE_CALL_BACK(LoginUserManager, this, ReqLogin);
}

void LoginUserManager::OnReqRsaPublicKey(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqRsaPublicKey>&) {
	auto pSend = std::make_shared<RspRsaPublicKey>();
	if (pSend) {
		pSend->set_publickey(_public_key);

		pConn->AsyncSend(pSend);
	}
}

void LoginUserManager::OnReqRegister(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqRegister>& pRecv) {
	std::string account = pRecv->account();
	std::string password = RSADecrypt(_private_key, pRecv->password());

	log(debug, "LoginUserManager") << "register account: " << account << ", password: " << password;

	if (VerifyAccount(account)) {
		std::shared_ptr<LoginUser> pUser = CreateUser(account);
		if (SaveUser(pUser, account, password)) {
			SendRegisterResult(pConn, 0); // success
			_userManager.insert({ pUser->GetID(), pUser });

			log(debug, "LoginUserManager") << "create user, user id: " << pUser->GetID();
			log(debug, "LoginUserManager") << "user manager size: " << _userManager.size();
		}
	}
	else SendRegisterResult(pConn, 1); // alread existed
}

bool LoginUserManager::SendRegisterResult(const TcpConnectionPtr& pConn, int32_t result) {
	auto pSend = std::make_shared<RegisterResult>();
	if (pSend) {
		pSend->set_result(result);

		pConn->AsyncSend(pSend);
	}

	return true;
}


void LoginUserManager::OnReqLogin(const TcpConnectionPtr& pConn, const std::shared_ptr<ReqLogin>& pRecv) {
	std::string account = pRecv->account();
	std::string password = RSADecrypt(_private_key, pRecv->password());
	std::string inputPassword = PBKDFEncrypt(password, SHA3_256Hash(password));

	std::ostringstream oss;

	oss << "SELECT user.`id`, user.`pwd` FROM x_login.user WHERE user.`act` = '" << account << "';";

	auto pSend = std::make_shared<LoginResult>();

	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((uint32_t)DBType::x_login)->ExecuteSql(oss.str());
	if (result.count() == 0) pSend->set_result(1);
	else {
		assert(result.count() == 1);
		Row row = result.fetchOne();
		assert(!row.isNull());
		uint64_t userid = static_cast<uint64_t>(row.get(0));
		std::string existPassword = static_cast<std::string>(row.get(1));
		if (inputPassword == existPassword) {
			std::shared_ptr<LoginUser> pUser = std::make_shared<LoginUser>();
			pUser->SetID(userid);
			pUser->SetState(LoginState::ONLINE);
			pUser->SetLastAccessTime(Now::Second());
			pUser->SetConnection(pConn);
			_userManager[userid] = pUser;

			pSend->set_result(0);
			pSend->set_act_id(userid);

			ZoneServerManager::GetInstance().SendAllZoneList(pConn);
		}
		else {
			pSend->set_result(2);
		}
	}

	pConn->AsyncSend(pSend);
}

bool LoginUserManager::VerifyAccount(const std::string& account) {
	std::ostringstream oss;

	oss << "SELECT user.`act` FROM x_login.user WHERE user.`act` = '" << account << "';";
	
	DBServicePtr& pDBService = LoginServer::GetInstance().GetDBService();
	SqlResult result = pDBService->GetDBConnectionByType((uint32_t)DBType::x_login)->ExecuteSql(oss.str());

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
	pDBService->GetDBConnectionByType((uint32_t)DBType::x_login)->ExecuteSql(oss.str());

	return true;
}
