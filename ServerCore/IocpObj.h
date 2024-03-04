#pragma once


class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(class IocpEvent* iocpEvent, int numOfBytes = 0) abstract;
};

