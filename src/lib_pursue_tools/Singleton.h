#pragma once

#include <memory>
#include <mutex>

namespace piece {
namespace tool {

template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts&& ... params) {
	return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

template <typename T>
class Singleton {
public:
	Singleton(const Singleton&) = delete;
	Singleton& operator=(const Singleton&) = delete;

protected:
	Singleton() {}
	~Singleton() {}

public:
	static T& GetInstance() {
		std::call_once(_onceFlag, &Singleton::Make);
		return *_instance;
	}

private:
	static void Make() {
		_instance = make_unique<T>();
	}

private:
	static std::once_flag _onceFlag;
	static std::unique_ptr<T> _instance;
};

template <typename T> std::once_flag Singleton<T>::_onceFlag;
template <typename T> std::unique_ptr<T> Singleton<T>::_instance;

}
}
