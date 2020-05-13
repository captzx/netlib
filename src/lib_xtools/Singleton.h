#pragma once

#include <memory>
#include <mutex>

#include "Type.h"

namespace x {

namespace tool {

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
		_instance = std::make_unique<T>();
	}

private:
	static std::once_flag _onceFlag;
	static std::unique_ptr<T> _instance;
};

template <typename T> std::once_flag Singleton<T>::_onceFlag;
template <typename T> std::unique_ptr<T> Singleton<T>::_instance;

} // namespace tool

} // namespace x
