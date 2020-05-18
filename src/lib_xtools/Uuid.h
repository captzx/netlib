#pragma once

namespace x {

namespace tool {

class UuidGenerator {
public:
	UuidGenerator();

public:
	void Init(long worker, long type, long seq = 0L);
	long long Get();

private:
	long long WaitToNextMillis(long long last_time);

private:
	long _worker;
	long _type;
	long _seq;
	long _last_time;
};

}

}