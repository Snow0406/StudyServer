#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
#include "IocpObj.h"

IocpCore::IocpCore()
{
	//»ý¼º
	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
}

IocpCore::~IocpCore()
{
	CloseHandle(iocpHandle);
}

bool IocpCore::Register(IocpObj* iocpObj)
{
	if (iocpObj->GetHandle() == nullptr)
		return false;

	CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, 0, 0);

	return true;
}

bool IocpCore::ObserveIO(DWORD time)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = nullptr;

	if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&iocpEvent, time))
	{
		IocpObj* iocpObj = iocpEvent->iocpObj;
		iocpObj->ObserveIO(iocpEvent, bytesTransferred);
	}
	else
	{
		switch (GetLastError())
		{
		case WAIT_TIMEOUT:
			return false;
		default:
			break;
		}

		return false;
	}


	//Todo

	return true;
}