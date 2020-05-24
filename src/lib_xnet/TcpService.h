#pragma once 

#include <boost/asio.hpp>

#include "UseTools.h"
#include "Buffer.h"
#include "ProtobufProcess.h"

using boost::asio::ip::tcp;
using boost::asio::io_context;
using SocketPtr = std::shared_ptr<tcp::socket>;
using boost::system::error_code;
using x::tool::NoCopyable;

namespace x {

namespace net {

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer&)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;
using CloseCallback = std::function<void(const TcpConnectionPtr&)>;

using MessagePtr = std::shared_ptr<google::protobuf::Message>;

void DefaultMessageCallback(const TcpConnectionPtr&, Buffer&);
void DefaultConnectionCallback(const TcpConnectionPtr&);
void DefaultCloseCallback(const TcpConnectionPtr&);

/// TcpConnection
class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
public:
	enum State {
		Unused,
		Connected,
		Disconnected
	};
public:
	TcpConnection(tcp::socket);
	~TcpConnection();
public:
	void OnEstablish();
	void Disconnect();
	void AsyncSend(const MessagePtr& pMessage);
	void AsyncReceive();

	bool IsConnectioned() { return _state == Connected; }
	tcp::socket& GetSocket() { return _sock; }

	unsigned int GetLastHeartBeatTime() { return _lastHeartBeat; }
	void SetLastHeartBeatTime(unsigned int time) { _lastHeartBeat = time; }
	unsigned int GetPid() { return _pid; }
	std::string GetLocalEndpoint() { 
		std::ostringstream oss;
		oss << _sock.local_endpoint();
		return oss.str();
	}
	unsigned int GetStartTime() { return _startTime; }
	void SetID(int id) { _id = id; }
	int GetID() { return _id; }
	void SetType(unsigned int type) { _type = type; }

public:
	void SetMessageCallback(MessageCallback callback) { _messageCallback = callback; }
	void SetCloseCallback(CloseCallback callback) { _closeCallback = callback; }
	
private:
	tcp::socket _sock;

	int _id;
	unsigned int _type; 
	unsigned int _pid;
	unsigned int _startTime;
	State _state;

	Buffer _buf;

	unsigned int _lastHeartBeat;

private:
	MessageCallback _messageCallback;
	CloseCallback _closeCallback;

public:
	static int Count;
};
using TcpConnectionManager = std::map<int, TcpConnectionPtr>;

/// TcpService
class TcpService {
public:
	explicit TcpService(std::string);

public:
	void AsyncListen(unsigned int);
	TcpConnectionPtr AsyncConnect(std::string ip, unsigned int port);

	void RemovePassiveConnection(const TcpConnectionPtr&);
	void RemoveActiveConnection(const TcpConnectionPtr&);
	
	void SetMessageCallback(MessageCallback callback) {
		_messageCallback = callback;
	}
	void SetConnectionCallback(ConnectionCallback callback) {
		_connectionCallback = callback;
	}

	void Start();
	void Close();
private:
	void AsyncHeartBeatInLoop();
	void AsyncListenInLoop();

public:
	unsigned int GetActiveConnectCount() { return _activeConnections.size(); }
	unsigned int GetPassiveConnectCount() { return _passiveConnections.size(); }
	TcpConnectionManager& GetActiveConnectionMgr() { return _activeConnections; }
	TcpConnectionManager& GetPassiveConnectionMgr() { return _passiveConnections; }
	io_context& GetIOContext() { return _io_context; }
private:
	std::string _name;
	io_context _io_context;
	tcp::acceptor _acceptor;
	boost::asio::deadline_timer _heartTimer;

	TcpConnectionManager _activeConnections;
	TcpConnectionManager _passiveConnections;

	MessageCallback _messageCallback;
	ConnectionCallback _connectionCallback;

	std::thread _io_thread;
};
using TcpServicePtr = std::shared_ptr<TcpService>;

} // namespace net

} // namespace x