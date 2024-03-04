#include "pch.h"
#include "IocpCore.h"'
#include "Session.h"
#include "IocpEvent.h"
#include "IocpObj.h"

IocpCore::IocpCore()
{
	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
}

IocpCore::~IocpCore()
{
	CloseHandle(iocpHandle);
}

void IocpCore::Register(class IocpObj* iocpObj)
{
	CreateIoCompletionPort(iocpObj->GetHandle(), iocpHandle, 0, 0);
}

bool IocpCore::ObserverIO(DWORD time)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	IocpEvent* iocpEvent = NULL;

		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&iocpEvent, time))
		{
			IocpObj* iocpObj = iocpEvent->iocpObj;
			iocpObj->ObserveIO(iocpEvent, bytesTransferred);
			//printf("Client Connected...\n");
		}
		else {
			switch (GetLastError())
			{
			case WAIT_TIMEOUT:
				return false;
			default:
				break;
			}
			printf("GetQueuedCompletionStatus Error : %d\n", WSAGetLastError());
			return false;
		}
	return true;
}
