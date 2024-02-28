#pragma once

class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;
	HANDLE iocpHandle = NULL;
	LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
public:
	Listener();
	~Listener();
public:
	HANDLE GetHandle() { return iocpHandle; }
public:
	bool StartAccept(class Service& service);
	void CloseSocket();
};

