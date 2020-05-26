#pragma once
#include <xprotos/Login.pb.h>

#include "LoginServer.h"

using x::tool::Singleton;
using x::tool::UuidGenerator;

using namespace x::net;

namespace x {

namespace login {

enum class LoginState {
	OFFLINE = 0,
	ONLINE = 1,
};

class LoginUser {
public:
	LoginUser();

public:
	long long GetID() { return _id; }
	void SetID(ull id) { _id = id; }
	void SetState(LoginState state) { _state = state; }
	void SetLastAccessTime(unsigned int timestamp) { _lastAccessTime = timestamp; }

	void SetConnection(const TcpConnectionPtr& pConnection) { _wpConnection = pConnection; }
	TcpConnectionWeakPtr& GetConnection() { return _wpConnection; }
private:
	ull _id;
	LoginState _state;
	unsigned int _lastAccessTime;

	TcpConnectionWeakPtr _wpConnection;
};

class LoginUserManager : public Singleton<LoginUserManager> {
public:
	LoginUserManager();

public:
	void Init();
	std::shared_ptr<LoginUser> FindUser(ull actid);

public:
	void RegisterMessageCallback();

public:
	void OnRequestRsaPublicKey(const TcpConnectionPtr&, const std::shared_ptr<RequestRsaPublicKey>&);

	void OnRequestRegister(const TcpConnectionPtr&, const std::shared_ptr<RequestRegister>&);
	bool SendRegisterResult(const TcpConnectionPtr&, int result);

	void OnRequestLogin(const TcpConnectionPtr&, const std::shared_ptr<RequestLogin>&);
public:
	bool VerifyAccount(const std::string& account);
	bool VerifyPassword(const std::string& account, const std::string& password);
	std::shared_ptr<LoginUser> CreateUser(const std::string& account);
	bool SaveUser(const std::shared_ptr<LoginUser>& pUser, const std::string& account, const std::string& password);

private:
	std::string _private_key;
	std::string _public_key;
	UuidGenerator _uuidGenerator;
	std::map<ull, std::shared_ptr<LoginUser>> _userManager;
};

}

}