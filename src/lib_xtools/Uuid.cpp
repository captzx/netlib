#include "Uuid.h"

#include <cassert>
#include "Time.h"

using namespace x::tool;

const long TWEPOCH = 1288834974657L;

const int WORJER_ID_BITS = 5;
const int DATA_CENTER_ID_BITS = 5;
const int SEQUENCE_BITS = 12;

const long MAX_WORKER_ID = -1L ^ (-1L << WORJER_ID_BITS);
const long MAX_DATA_CENTER_ID = -1L ^ (-1L << DATA_CENTER_ID_BITS);

const int WORKER_ID_L_SHIFT = SEQUENCE_BITS;
const int DATA_CENTER_ID_L_SHIFT = SEQUENCE_BITS + WORJER_ID_BITS;
const int TIME_STAMP_L_SHIFT = SEQUENCE_BITS + WORJER_ID_BITS + DATA_CENTER_ID_BITS;

const long SEQUENCE_MASK = -1L ^ (-1L << SEQUENCE_BITS);

UuidGenerator::UuidGenerator() : _worker(0), _type(0), _seq(0), _last_time(-1L) {

}

void UuidGenerator::Init(long worker, long type, long seq) {
	assert(0 <= worker && worker <= MAX_WORKER_ID);
	assert(0 <= type && type <= MAX_DATA_CENTER_ID);

	_worker = worker;
	_type = type;
	_seq = seq;
}

long long UuidGenerator::Get() {
	long long now = Now::MilliSecond();
	assert(now >= _last_time);

	if (_last_time == now) {
		_seq = (_seq + 1) & SEQUENCE_MASK;

		if (_seq == 0) now = WaitToNextMillis(_last_time);
	}
	else _seq = 0;

	_last_time = now;

	long long id = ((now - TWEPOCH) << TIME_STAMP_L_SHIFT) | (_type << DATA_CENTER_ID_L_SHIFT) | (_worker << WORKER_ID_L_SHIFT) | _seq;

	return id;
}

long long UuidGenerator::WaitToNextMillis(const long long last_time) {
	long long  now = Now::MilliSecond();
	while (now <= last_time) now = Now::MilliSecond();
	return now;
}