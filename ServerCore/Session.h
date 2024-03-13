#pragma once
#include "IocpObj.h"
class Session : public IocpObj
{
	friend class Listener;
private:
	atomic<bool> connected = false;
	class Service* service = NULL;
	SOCKET socket = INVALID_SOCKET;
	SOCKADDR_IN sockAddr = {};
private:
	RecvEvent recvEvent;
public:
	//WSAOVERLAPPED overlapped = {};
	char recvBuffer[1024] = {};
public:
	Session();
	virtual ~Session();
private:

	// Inherited via IocpObj
	HANDLE GetHandle() override;
	void ObserveIO(IocpEvent* iocpEvent, int numOfBytes) override;
public:
	SOCKET GetSocket() const { return socket; }
	bool isConnected() const { return connected; }
	Service* GetService() const { return service; }
public:
	void SetService(Service* _service) { service = _service; }
	void SetSockAddr(SOCKADDR_IN address) { sockAddr = address; }
private:
	void RegisterRecv();
private:
	void ProcessConnect();
	void ProcessRecv(int numOfBytes);
private:
	void HandleError(int errorCode);
protected:
	virtual void OnConnected() {}
	virtual int OnRecv(BYTE* buffer, int len) { return len; }
};

