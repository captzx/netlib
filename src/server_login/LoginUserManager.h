#pragma once
#include <xprotos/Login.pb.h>

#include "LoginServer.h"

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
	uint64_t GetID() { return _id; }
	void SetID(uint64_t id) { _id = id; }
	void SetState(LoginState state) { _state = state; }
	void SetLastAccessTime(uint32_t timestamp) { _lastAccessTime = timestamp; }

	void SetConnection(const TcpConnectionPtr& pConnection) { _wpConnection = pConnection; }
	TcpConnectionWeakPtr& GetConnection() { return _wpConnection; }

	void SetZoneAddress(std::pair<std::string, uint32_t> address) { _address = address; }
	std::pair<std::string, uint32_t> GetZoneAddress() { return _address; }
private:
	uint64_t _id;
	LoginState _state;
	uint32_t _lastAccessTime;

	TcpConnectionWeakPtr _wpConnection;
	std::pair<std::string, uint32_t> _address;
};

class LoginUserManager : public Singleton<LoginUserManager> {
public:
	LoginUserManager();

public:
	void Init();
	std::shared_ptr<LoginUser> FindUser(uint64_t actid);

public:
	void RegisterMessageCallback();

public:
	void OnReqRsaPublicKey(const TcpConnectionPtr&, const std::shared_ptr<ReqRsaPublicKey>&);
	void OnReqRegister(const TcpConnectionPtr&, const std::shared_ptr<ReqRegister>&);
	void OnReqLogin(const TcpConnectionPtr&, const std::shared_ptr<ReqLogin>&);

	bool SendRegisterResult(const TcpConnectionPtr&, int32_t result);

public:
	bool VerifyAccount(const std::string& account);
	bool VerifyPassword(const std::string& account, const std::string& password);
	std::shared_ptr<LoginUser> CreateUser(const std::string& account);
	bool SaveUser(const std::shared_ptr<LoginUser>& pUser, const std::string& account, const std::string& password);

private:
	std::string _private_key;
	std::string _public_key;
	UuidGenerator _uuidGenerator;
	std::pair<std::string, uint32_t> _address;
	std::map<uint64_t, std::shared_ptr<LoginUser>> _userManager;
};

}

}