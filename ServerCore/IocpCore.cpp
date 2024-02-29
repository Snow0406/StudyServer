#include "pch.h"
#include "IocpCore.h"'
#include "Session.h"

IocpCore::IocpCore()
{
	iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
}

IocpCore::~IocpCore()
{
	CloseHandle(iocpHandle);
}

void IocpCore::Register(HANDLE socket, ULONG_PTR key)
{
	CreateIoCompletionPort((HANDLE)socket, iocpHandle, key, 0);
}

bool IocpCore::ObserverIO(DWORD time)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	WSAOVERLAPPED* overlapped = {};

		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
		{
			printf("Client Connected...\n");
		}
		else {
			printf("GetQueuedCompletionStatus Error : %d\n", WSAGetLastError());
			return false;
		}
	return true;
}
