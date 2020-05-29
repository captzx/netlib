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
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
#define BOOST_BIND_NO_PLACEHOLDERS // avoid placeholders ambiguous

#include <thread>
#include <mutex>
#include <condition_variable>

#include <chrono>

#include <cassert>

#include <fstream>
#include <iomanip>

#include <regex>
// c
#include "string.h"

// linux
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/timerfd.h>