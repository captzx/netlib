#pragma once
#include <xtools/Uuid.h>
#include <xprotos/Login.pb.h>

#include "LoginServer.h"

using x::tool::Singleton;
using x::tool::UuidGenerator;

using namespace x::net;

namespace x {

namespace login {

class LoginUser;
class LoginUserManager : public Singleton<LoginUserManager> {
public:
	LoginUserManager();

public:
	void Init();

public:
	void RegisterMessageCallback(ProtobufDispatcher&);

public:
	void OnRequestRsaPublicKey(const TcpConnectionPtr& pConnection, const std::shared_ptr<RequestRsaPublicKey>&);

	void OnRequestRegister(const TcpConnectionPtr&, const std::shared_ptr<RequestRegister>&);
	bool SendRegisterResult(const TcpConnectionPtr& pConnection, int result);

	void OnRequestLogin(const TcpConnectionPtr&, const std::shared_ptr<RequestLogin>&);
	bool SendLoginResult(const TcpConnectionPtr& pConnection, int result);

public:
	bool VerifyAccount(const std::string& account);
	bool VerifyPassword(const std::string& account, const std::string& password);
	std::shared_ptr<LoginUser> CreateUser(const std::string& account);
	bool SaveUser(const std::shared_ptr<LoginUser>& pUser, const std::string& account, const std::string& password);

private:
	std::string _private_key;
	std::string _public_key;
	UuidGenerator _uuidGenerator;
	std::map<long long, std::shared_ptr<LoginUser>> _userManager;
};

}

}