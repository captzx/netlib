#pragma once

#include "Using.h"

namespace x {
namespace net {

class Buffer {
public:
	static const size_t PrependSize = 8;
	static const size_t InitialSize = 1024;

public:
	explicit Buffer(size_t len = InitialSize)
		:_buf(PrependSize + InitialSize),
		_rptr(PrependSize),
		_wptr(PrependSize) {

	}
public:
	template <typename T>
	T Read() {
		T result = 0;
		std::memcpy(&result, ReadPtr(), sizeof(T));
		_rptr += sizeof(T);
		return result;
	}

	std::string Read(size_t len) {
		std::string result;
		result.resize(len);
		std::memcpy(&result[0], ReadPtr(), len);
		_rptr += len;
		return result;
	}

	// write
	char* BeginPtr() {
		return &_buf[0];
	}
	char* WritePtr() {
		return BeginPtr() + _wptr;
	}
	char* ReadPtr() {
		return BeginPtr() + _rptr;
	}

	void EnsureWriteable(size_t len) {
		if (Writeable() < len) MakeMoreWriteSpace(len);
	}
	unsigned int Readable() {
		return _wptr - _rptr;
	}
	unsigned int Writeable() {
		return _buf.size() - _wptr;
	}
	unsigned int AlreadyRead() {
		return _rptr;
	}
	void MakeMoreWriteSpace(size_t len) {
		if (AlreadyRead() + Writeable() < len + PrependSize) {
			_buf.resize(_wptr + len);
		}
		else {
			unsigned int readable = Readable();
			std::copy(ReadPtr(), WritePtr(), BeginPtr() + PrependSize);
			_rptr = PrependSize;
			_wptr = _rptr + readable;
		}
	}
	void Write(const void* data, size_t len) {
		EnsureWriteable(len);
		const char* data_ = static_cast<const char*>(data);
		std::copy(data_, data_ + len, WritePtr());
		_wptr += len;
	}

	template <typename T>
	void Write(T data) {
		static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value,
			"Generic Write only support primitive data types");

		Write(&data, sizeof(data));
	}

	void MoveWritePtr(size_t len) {
		_wptr += len;
	}
	void Prepend(const void* data, size_t len) {
		_rptr -= len;
		const char* d = static_cast<const char*>(data);
		std::copy(d, d + len, BeginPtr() + _rptr);
	}

	void Retrieve(unsigned int len) {
		if (len < Readable()) {
			_rptr += len;
		}
		else
		{
			_rptr = PrependSize;
			_wptr = PrependSize;
		}
	}
private:
	std::vector<char> _buf;
	size_t _rptr;
	size_t _wptr;
};

} // namespace net
} // namespace x