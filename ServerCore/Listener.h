#pragma once
#include "IocpObj.h"

class ServerService;
class AcceptEvent;

class Listener : public IocpObj
{
private:
	ServerService* serverService = nullptr;
	SOCKET socket = INVALID_SOCKET;
public:
	Listener() = default;
	virtual ~Listener();
public:
	bool StartAccept(ServerService* service);
	void CloseSocket();
public:
	void RegisterAccept(AcceptEvent* acceptEvent); //Accept ���
	void ProcessAccept(AcceptEvent* acceptEvent); //Accept ����

	// Inherited via IocpObj
	HANDLE GetHandle() override;
	void ObserveIO(IocpEvent* iocpEvent, int numOfBytes) override;
};

