#pragma once

#include "Using.h"
#include "Buffer.h"

namespace x {

namespace net {

class TcpConnection : public std::enable_shared_from_this<TcpConnection> {
public:
	TcpConnection(SocketPtr);

	void Established(); 
	// void Disconnected();

	void Close();
	// void Shutdown();

	void AsyncSend(Buffer&);
	// void SyncReceive();
	void AsyncReceive();

public:
	void SetMessageCallback(MessageCallback callback) {
		_messageCallback = callback;
	}
	// void DefaultMessageCallback(const TcpConnectionPtr&, const Buffer&);

private:
	SocketPtr _pSock;
	Buffer _buf;

private:
	MessageCallback _messageCallback;
};


} // namespace net
} // namespace x