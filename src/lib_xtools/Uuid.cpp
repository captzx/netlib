#include "Uuid.h"

#include <cassert>
#include "Time.h"

using namespace x::tool;

const uint64_t TWEPOCH = 1288834974657L;

const int32_t WORJER_ID_BITS = 5;
const int32_t DATA_CENTER_ID_BITS = 5;
const int32_t SEQUENCE_BITS = 12;

const int32_t MAX_WORKER_ID = -1L ^ (-1L << WORJER_ID_BITS);
const int32_t MAX_DATA_CENTER_ID = -1L ^ (-1L << DATA_CENTER_ID_BITS);

const int32_t WORKER_ID_L_SHIFT = SEQUENCE_BITS;
const int32_t DATA_CENTER_ID_L_SHIFT = SEQUENCE_BITS + WORJER_ID_BITS;
const int32_t TIME_STAMP_L_SHIFT = SEQUENCE_BITS + WORJER_ID_BITS + DATA_CENTER_ID_BITS;

const int32_t SEQUENCE_MASK = -1L ^ (-1L << SEQUENCE_BITS);

UuidGenerator::UuidGenerator() : _worker(0), _type(0), _seq(0), _last_time(0) {

}

void UuidGenerator::Init(int32_t worker, int32_t type, int32_t seq) {
	assert(0 <= worker && worker <= MAX_WORKER_ID);
	assert(0 <= type && type <= MAX_DATA_CENTER_ID);

	_worker = worker;
	_type = type;
	_seq = seq;
}

uint64_t UuidGenerator::Get() {
	uint64_t now = Now::MilliSecond();
	assert(now >= _last_time);

	if (_last_time == now) {
		_seq = (_seq + 1) & SEQUENCE_MASK;

		if (_seq == 0) now = WaitToNextMillis(_last_time);
	}
	else _seq = 0;

	_last_time = now;

	uint64_t id = ((now - TWEPOCH) << TIME_STAMP_L_SHIFT) | (_type << DATA_CENTER_ID_L_SHIFT) | (_worker << WORKER_ID_L_SHIFT) | _seq;

	return id;
}

uint64_t UuidGenerator::WaitToNextMillis(const uint64_t last_time) {
	uint64_t  now = Now::MilliSecond();
	while (now <= last_time) now = Now::MilliSecond();
	return now;
}