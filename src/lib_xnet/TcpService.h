#pragma once 

#include <boost/asio.hpp>

#include "UseTools.h"
#include "Buffer.h"
#include "ProtobufProcess.h"

using boost::asio::ip::tcp;
using boost::asio::io_context;
using boost::system::error_code;

using x::tool::NoCopyable;

namespace x {

namespace net {

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using TcpConnectionWeakPtr = std::weak_ptr<TcpConnection>;

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

private:
	tcp::socket _sock;
	Buffer _buf;
public:
	tcp::socket& GetSocket() { return _sock; }

public:
	static int32_t Count;

private:
	State _state;
	int32_t _id;
	uint32_t _pid;
	uint32_t _startTime;
public:
	std::string ToString() {
		std::ostringstream oss;
		if (_state == Connected) {
			oss << "state: connected | id: " << _id << " | pid: " << _pid << " | establish time: " << _startTime
				<< " | local: " << _sock.local_endpoint() << " | remote: " << _sock.remote_endpoint();
		}
		else oss << "connection not usable.";

		return oss.str();
	}
	uint32_t GetPid() { return _pid; }
	std::string GetLocalEndpoint() {
		std::ostringstream oss;
		oss << _sock.local_endpoint();
		return oss.str();
	}
	uint32_t GetStartTime() { return _startTime; }
	void SetID(int32_t id) { _id = id; }
	int32_t GetID() { return _id; }

private:
	uint32_t _lastHeartBeat;
public:
	uint32_t GetLastHeartBeatTime() { return _lastHeartBeat; }
	void SetLastHeartBeatTime(uint32_t time) { _lastHeartBeat = time; }

private:
	MessageCallback _messageCallback;
	CloseCallback _closeCallback;
public:
	void SetMessageCallback(MessageCallback callback) { _messageCallback = callback; }
	void SetCloseCallback(CloseCallback callback) { _closeCallback = callback; }
};

/// TcpConnectionManager
class TcpConnectionManager {
public:
	TcpConnectionManager() {}
	~TcpConnectionManager() {}

private:
	std::map<int32_t, TcpConnectionPtr> _connections;
public:
	void Add(TcpConnectionPtr pConn);
	std::map<int32_t, TcpConnectionPtr>& GetAll() { return _connections; }
	uint32_t GetConnectedCount() { return _connections.size(); }
	void RemoveConnection(const TcpConnectionPtr& pConnection);
	void CloseAllConnection();
};

/// TcpService
class TcpService {
public:
	explicit TcpService(std::string);

private:
	std::string _name;

	io_context _io_context;
	tcp::acceptor _acceptor;
	std::thread _io_thread;

	TcpConnectionManager _atvConnMgr;
	TcpConnectionManager _psvConnMgr;
public:
	io_context& GetIOContext() { return _io_context; }

	void AsyncListen(uint32_t);
	TcpConnectionPtr AsyncConnect(std::string ip, uint32_t port);

	void Start();
	void Close();

	TcpConnectionManager& GetAtvConnMgr() { return _atvConnMgr; }
	TcpConnectionManager& GetPsvConnMgr() { return _psvConnMgr; }

private:
	MessageCallback _messageCallback;
	ConnectionCallback _atvConnCallback;
	ConnectionCallback _psvConnCallback;
public:
	void SetMessageCallback(MessageCallback callback) { _messageCallback = callback; }
	void SetAtvConnCallback(ConnectionCallback callback) { _atvConnCallback = callback;	}
	void SetPsvConnCallback(ConnectionCallback callback) { _psvConnCallback = callback; }

private:
	void AsyncHeartBeatInLoop();
	void AsyncListenInLoop();
};
using TcpServicePtr = std::shared_ptr<TcpService>;

} // namespace net

} // namespace x