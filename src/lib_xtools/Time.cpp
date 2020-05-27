#include "Time.h"

#include <chrono>

using namespace x::tool;

uint32_t Now::Second() {
	return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
uint64_t Now::MilliSecond() {
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}