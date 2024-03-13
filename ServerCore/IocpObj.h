#pragma once
#include "IocpEvent.h"

class IocpObj
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void ObserveIO(IocpEvent* iocpEvent, int numOfBytes = 0) abstract;
};

