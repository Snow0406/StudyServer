#pragma once
class IocpCore;
class Listener;

class Service
{
private:
	SOCKADDR_IN sockAddr = {};
	Listener* listener = NULL;
	IocpCore* iocpCore = NULL;
public:
	Service(wstring ip, u_short port);
	~Service();
public:
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	IocpCore* GetIocpCore() { return iocpCore;  }
public:
	bool Start();
};