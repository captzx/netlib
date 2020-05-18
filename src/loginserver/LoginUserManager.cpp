#include "LoginUserManager.h"

#include "LoginUser.h"

#include <xtools/Time.h>

using namespace x::login;
using namespace x::tool;

LoginUserManager::LoginUserManager() {
	
}

void LoginUserManager::Init() {
	x::tool::GenerateRSAKey(_private_key, _public_key);
	_uuidGenerator.Init(0, 0);
}

void LoginUserManager::RegisterMessageCallback(ProtobufDispatcher& dispatcher) {
	dispatcher.RegisterMessageCallback<RequestRsaPublicKey>(std::bind(&LoginUserManager::OnRequestRsaPublicKey, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RequestRegister>(std::bind(&LoginUserManager::OnRequestRegister, this, std::placeholders::_1, std::placeholders::_2));
	dispatcher.RegisterMessageCallback<RequestLogin>(std::bind(&LoginUserManager::OnRequestLogin, this, std::placeholders::_1, std::placeholders::_2));
}

void LoginUserManager::OnRequestRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRsaPublicKey>&) {
	auto pMsg = std::make_shared<ResponseRsaPublicKey>();
	if (pMsg) {
		pMsg->set_publickey(_public_key);

		Buffer buf;
		ProtobufCodec::PackMessage(pMsg, buf);
		pConnection->AsyncSend(buf);
	}
}

void LoginUserManager::OnRequestRegister(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRegister>& pMsg) {
	std::string account = pMsg->account();
	std::string password = RSADecrypt(_private_key, pMsg->password());

	log(debug, "LoginUserManager") << "register account: " << account << ", password: " << password;

	if (VerifyAccount(account)) {
		std::shared_ptr<LoginUser> pUser = CreateUser(account);
		if (SaveUser(pUser, password)) {
			SendRegisterResult(pConnection, 1);
			_userManager.insert({ pUser->GetID(), pUser });
		}
	}
}

bool LoginUserManager::SendRegisterResult(const TcpConnectionPtr& pConnection, int result) {
	auto pMsg = std::make_shared<RegisterResult>();
	if (pMsg) {
		pMsg->set_result(result);

		Buffer buf;
		ProtobufCodec::PackMessage(pMsg, buf);
		pConnection->AsyncSend(buf);
	}

	return true;
}


void LoginUserManager::OnRequestLogin(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestLogin>& pMsg) {
	std::string account = pMsg->account();
	std::string password = RSADecrypt(_private_key, pMsg->password());
	std::string inputPassword = PBKDFEncrypt(password, SHA3_256Hash(password));
}

bool LoginUserManager::SendLoginResult(const TcpConnectionPtr& pConnection, int result) {
	auto pMsg = std::make_shared<LoginResult>();
	if (pMsg) {
		pMsg->set_result(result);

		Buffer buf;
		ProtobufCodec::PackMessage(pMsg, buf);
		pConnection->AsyncSend(buf);
	}

	return true;
}

bool LoginUserManager::VerifyAccount(const std::string& account) {
	log(debug, "LoginUserManager") << "todo: verify account";
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

bool LoginUserManager::SaveUser(const std::shared_ptr<LoginUser>& pUser, const std::string& password) {
	std::string result = PBKDFEncrypt(password, SHA3_256Hash(password)); // fix, don't use password generate salt

	log(debug, "LoginUserManager") << "PBKDFEncrypt result:" << result;
	log(debug, "LoginUserManager") << "todo: save account to db";
	return true;
}
