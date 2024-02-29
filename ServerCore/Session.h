#pragma once
class Session
{
private:
	SOCKET socket = INVALID_SOCKET;
public:
	WSAOVERLAPPED overlapped = {};
	char recvBuffer[1024] = {};
public:
	Session();
	~Session();
public:
	SOCKET GetSOcket() { return socket;  }
}

