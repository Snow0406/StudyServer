#pragma once
#include "IocpObj.h"

class Listener : public IocpObj
{
private:
	SOCKET socket = INVALID_SOCKET;
public:
	Listener() = default;
	~Listener();
public:
	bool StartAccept(class Service* service);
	void CloseSocket();
public:
	void RegisterAccpet(class AcceptEvent* acceptEvent); //Accept 등록
	void ProcessAccept(AcceptEvent* acceptEvent); //Accept 진행

	// Inherited via IocpObj
	HANDLE GetHandle() override;
	void ObserveIO(IocpEvent* iocpEvent, int numOfBytes) override;
};

