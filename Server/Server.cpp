#include "pch.h"
#include <Service.h>


void AcceptThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	WSAOVERLAPPED* overlapped = {};

	while (true)
	{
		printf("Waiting...\n");
		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&overlapped, INFINITE))
		{
			printf("Client Connected...\n");
		}
	}
}


int main()
{
	printf("============= SERVER =============\n");

	Service service(L"127.0.0.1", 27015);

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}


	if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
	{
		printf("bind failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	if (listen(listenSocket, 10) == SOCKET_ERROR)
	{
		printf("listen failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSAGetLastError();
		return 1;
	}

	printf("listening...\n");


	DWORD dwBytes;
	LPFN_ACCEPTEX lpfnAcceptEx = NULL;
	GUID GuidAcceptEx = WSAID_ACCEPTEX;

	if (WSAIoctl(listenSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &GuidAcceptEx, sizeof(GuidAcceptEx),
		&lpfnAcceptEx, sizeof(lpfnAcceptEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIcottl failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (acceptSocket == INVALID_SOCKET)
	{
		printf("Accept Socket failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}

	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	thread t(AcceptThread, iocpHandle);


	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, key, 0);

	char lpOutputBuf[1024];
	WSAOVERLAPPED overlapped = {};

	if (!lpfnAcceptEx(listenSocket, acceptSocket, lpOutputBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("AcceptEx failed with error : %d\n", WSAGetLastError());
			closesocket(acceptSocket);
			closesocket(listenSocket);
			WSACleanup();
			return 1;

		}
	}

	t.join();

	closesocket(listenSocket);

	return 0;


}