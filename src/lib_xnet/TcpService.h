#pragma once 

#include "Using.h"
#include "Buffer.h"

namespace x {

namespace net {

using boost::asio::ip::tcp;
using boost::asio::io_context;

class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using MessageCallback = std::function<void(const TcpConnectionPtr&, Buffer&)>;
using ConnectionCallback = std::function<void(const TcpConnectionPtr&)>;

void DefaultMessageCallback(const TcpConnectionPtr&, Buffer&);
void DefaultConnectionCallback(const TcpConnectionPtr&);

/// TcpConnection
class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(SocketPtr);

public:
	void Established();
	void Close();
	void AsyncSend(Buffer&);
	void AsyncReceive();

public:
	void SetMessageCallback(MessageCallback callback) { _messageCallback = callback; }

private:
	SocketPtr _pSock;
	Buffer _buf;

private:
	MessageCallback _messageCallback;
};

/// TcpService
class TcpServices {
public:
	explicit TcpServices(std::string name):
	_name(name),
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
protected:
	std::string _name;
	io_context _io_context;

protected:
	MessageCallback _messageCallback;
	ConnectionCallback _connectionCallback;
};
using TcpServicesPtr = std::shared_ptr<TcpServices>;

/// TcpServer
class TcpServer : public TcpServices {
public:
	using TcpConnectionManager = std::map<int, TcpConnectionPtr>;

public:
	explicit TcpServer(std::string);

public:
	void Listen(unsigned int port);

private:
	void AsyncListenInLoop();

private:
	tcp::acceptor _acceptor;
	TcpConnectionManager _tcpConnections;
};
using TcpServerPtr = std::shared_ptr<TcpServer>;

/// TcpClient
class TcpClient : public TcpServices {
public:
	explicit TcpClient(std::string);

public:
	void AsyncConnect(std::string ip, unsigned int port);
	void AsyncSend(Buffer& buf);
	void Close();

private:
	TcpConnectionPtr _pConnection;
};
using TcpClientPtr = std::shared_ptr<TcpClient>;

} // namespace net

} // namespace x