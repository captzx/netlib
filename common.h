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

// c
#include "string.h"

// linux
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#include <sys/timerfd.h>


#define DEFAULT_PORT 5766
#define BACKLOG 10
#define MAXLINE 1024

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;

class InetAddress;
class TcpConnection;
class Buffer;
using NewConnectionCallBack = std::function<void(int fd, const std::shared_ptr<InetAddress>&)>;
using EventCallBack = std::function<void()>;
using ReadEventCallBack = std::function<void(int)>;
using ConnectionCallBack = std::function<void(const std::shared_ptr<TcpConnection>&)>;
using MessageCallBack = std::function<void(const std::shared_ptr<TcpConnection>&, Buffer*, int)>;
using CloseCallBack = std::function<void(const std::shared_ptr<TcpConnection>&)>;