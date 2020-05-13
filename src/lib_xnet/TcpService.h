#pragma once 

#include "Using.h"
#include "Buffer.h"

using  x::tool::NoCopyable;

namespace x {

namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_context;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer&)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;
using HeartBeatCallback = std::function<void(const TcpConnectionPtr&)>;

void DefaultMessageCallback(const TcpConnectionPtr&, Buffer&);
void DefaultConnectionCallback(const TcpConnectionPtr&);

/// IOContext
class IOContext :public NoCopyable {
public:
	void Run(); 
	void RunInThread();
	void Stop();
	io_context& Get();
	bool IsRunning();
private:
	std::thread _ctx_running;
	io_context _io_context;
};

/// TcpConnection
class TcpConnection : public NoCopyable, public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(tcp::socket);
	~TcpConnection();
public:
	void Established();
	void Disconnect();
	void AsyncSend(Buffer&);
	void AsyncReceive();

	bool IsConnectioned() { return _state; }
	tcp::socket& GetSocket() { return _sock; }
	unsigned int GetLastHeartBeatTime() { return _lastHeartBeat; }
	void SetLastHeartBeatTime(unsigned int time) { _lastHeartBeat = time; }
public:
	void SetID(unsigned int id) { _id = id; }
	unsigned int GetID() { return _id; }

public:
	void SetMessageCallback(MessageCallback callback) { _messageCallback = callback; }
	void SetCloseCallback(CloseCallback callback) { _closeCallback = callback; }
	
private:
	bool _state;
	unsigned int _id;
	tcp::socket _sock;
	Buffer _buf;
	unsigned int _lastHeartBeat;

private:
	MessageCallback _messageCallback;
	CloseCallback _closeCallback;
};

/// TcpService
class TcpServices : public NoCopyable {
public:
	explicit TcpServices(std::string name) :
	_name(name),
	_heartbeatPeriod(10), // default heart beat 10s
	_messageCallback(std::bind(&DefaultMessageCallback, std::placeholders::_1, std::placeholders::_2)),
	_connectionCallback(std::bind(&DefaultConnectionCallback, std::placeholders::_1)) {

	}

public:
	void SetMessageCallback(MessageCallback callback) {
		_messageCallback = callback;
	}
	void SetConnectionCallback(ConnectionCallback callback) {
		_connectionCallback = callback;
	}
	void SetHeartCallback(HeartBeatCallback callback) {
		_heartCallback = callback;
	}
	
protected:
	std::string _name;
	int _heartbeatPeriod;

protected:
	MessageCallback _messageCallback;
	ConnectionCallback _connectionCallback; 
	HeartBeatCallback _heartCallback;
};
using TcpServicesPtr = std::shared_ptr<TcpServices>;

/// TcpServer
class TcpServer : public TcpServices {
public:
	using TcpConnectionManager = std::map<int, TcpConnectionPtr>;

public:
	explicit TcpServer(IOContext&, std::string);

public:
	void Init();
	void Listen(unsigned int port);
	void RemoveConnection(const TcpConnectionPtr&);

private:
	void CheckHeartBeat();
	void AsyncHeartBeatInLoop();
	void AsyncListenInLoop();

private:
	int _counter;
	tcp::acceptor _acceptor;
	TcpConnectionManager _tcpConnections;
	boost::asio::deadline_timer _heartTimer;
};
using TcpServerPtr = std::shared_ptr<TcpServer>;

/// TcpClient
class TcpClient : public TcpServices {
public:
	explicit TcpClient(IOContext&, std::string);

public:
	bool IsConnectioned();
	void AsyncConnect(std::string ip, unsigned int port);
	void AsyncSend(Buffer& buf);
	void Close();
	void DestroyConnection(const TcpConnectionPtr&);
private:
	IOContext& _ctx;
	TcpConnectionPtr _pConnection;
};
using TcpClientPtr = std::shared_ptr<TcpClient>;

} // namespace net

} // namespace x