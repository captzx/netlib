#pragma once

#include "Type.h"

namespace x {

namespace tool {

class UuidGenerator {
public:
	UuidGenerator();

public:
	void Init(int32_t worker, int32_t type, int32_t seq = 0L);
	uint64_t Get();

private:
	uint64_t WaitToNextMillis(const uint64_t last_time);

private:
	int32_t _worker;
	int32_t _type;
	int32_t _seq;
	uint64_t _last_time;
};

}

}