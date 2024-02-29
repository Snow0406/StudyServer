#pragma once
class IocpCore
{
private:
	HANDLE iocpHandle = NULL;
public:
	IocpCore();
	~IocpCore();
public:
	HANDLE GetHandle() { return iocpHandle;  }
public:
	void Register(HANDLE socket, ULONG_PTR key);
	bool ObserverIO(DWORD time = INFINITE);
};

