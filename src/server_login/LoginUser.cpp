#include "LoginUser.h"


using namespace x::login;

LoginUser::LoginUser():
	_id(0),
	_state(LoginState::OFFLINE),
	_lastAccessTime(0) {

}