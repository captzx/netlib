#pragma once

// cpp
#include <iostream>
#include <sstream>

#include <vector>
#include <list>
#include <string>
#include <stack>
#include <queue>
#include <set>
#include <map>

#include <memory>

#include <functional> // function/bind

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include <cassert>

#include <fstream>
#include <iomanip>

// c
#include "string.h"

// linux
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/timerfd.h>

#include <zlib.h> // adler32

#include "Type.h"


namespace x {

namespace tool {

class NoCopyable {
protected:
	NoCopyable() {}
	~NoCopyable() {}
private:
	NoCopyable(const NoCopyable&);
	const NoCopyable& operator=(const NoCopyable&);
};

unsigned int GetSystemTime();

}

}