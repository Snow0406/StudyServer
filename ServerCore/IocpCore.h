#pragma once
class IocpCore
{
private:
	HANDLE iocpHandle = NULL;
public:
	IocpCore();
	~IocpCore();
public:
	HANDLE GetHandle() const { return iocpHandle; }
public:
	bool Register(class IocpObj* iocpObj);  // bool�� �ٲ�
	bool ObserveIO(DWORD time = INFINITE);
};