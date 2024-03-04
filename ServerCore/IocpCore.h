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
	void Register(class IocpObj* iocpObj);
	bool ObserverIO(DWORD time = INFINITE);
};

