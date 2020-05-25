#pragma once


#include "UseTools.h"
#include "UseNet.h"

using namespace x::tool;
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

}

}

