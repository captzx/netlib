#pragma once

#include "Type.h"

namespace x {

namespace tool {

class UuidGenerator {
public:
	UuidGenerator();

public:
	void Init(long worker, long type, long seq = 0L);
	ull Get();

private:
	ull WaitToNextMillis(const ull last_time);

private:
	long _worker;
	long _type;
	long _seq;
	ull _last_time;
};

}

}