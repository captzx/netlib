#pragma once

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
	void SetID(long long id) { _id = id; }
	void SetState(LoginState state) { _state = state; }
	void SetLastAccessTime(unsigned int timestamp) { _lastAccessTime = timestamp; }
private:
	long long _id;
	LoginState _state;
	unsigned int _lastAccessTime;
};

}

}

