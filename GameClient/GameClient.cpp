#include <iostream>			   

using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <thread>
#include <MSWSock.h> 

//IOCP 타입을 만듬
enum IOCP_TYPE
{
	NONE,
	CONNECT,
	DISCONNECT,

};

//Session을 만들어서 IOCP_TYPE을 추가
struct Session
{
	WSAOVERLAPPED overlapped = {};
	IOCP_TYPE type = NONE;
};


void ConnectThread(HANDLE iocpHandle)
{
	DWORD bytesTransferred = 0;
	ULONG_PTR key = 0;
	//WSAOVERLAPPED overlapped = {};
	Session* session = nullptr;

	while (true)
	{
		printf("Waiting...\n");

		if (GetQueuedCompletionStatus(iocpHandle, &bytesTransferred, &key, (LPOVERLAPPED*)&session, INFINITE))
		{
			switch (session->type)
			{
			case CONNECT:
				printf("Client Connect!\n");
				break;
			case DISCONNECT:
				printf("Client Disconnect!\n");
			default:
				break;
			}
		}
	}
}


int main()
{
	Sleep(1000);

	printf("============= CLIENT =============\n");

	WORD wVersionRequested;
	WSAData wsaData;

	wVersionRequested = MAKEWORD(2, 2);
	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup function failed with error\n");
		return 1;
	}


	SOCKET connectSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (connectSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error : %d\n", WSAGetLastError());
		WSACleanup();
		return 1;

	}

	//ConnectEx 함수포인터 로드
	DWORD dwBytes;
	LPFN_CONNECTEX lpfnConnectEx = NULL;
	GUID guidConnectEx = WSAID_CONNECTEX;
	if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidConnectEx, sizeof(guidConnectEx),
		&lpfnConnectEx, sizeof(lpfnConnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl ConnectEx load failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	//DisconnectEx 함수포인터 로드
	LPFN_DISCONNECTEX lpfnDisconnectEx = NULL;
	GUID guidDisConnectEx = WSAID_DISCONNECTEX;
	if (WSAIoctl(connectSocket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisConnectEx, sizeof(guidDisConnectEx),
		&lpfnDisconnectEx, sizeof(lpfnDisconnectEx), &dwBytes, NULL, NULL) == SOCKET_ERROR)
	{
		printf("WSAIoctl DisconnectEx load failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}


	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
	service.sin_port = htons(27015);


	SOCKADDR_IN localService;
	memset(&localService, 0, sizeof(localService));
	localService.sin_family = AF_INET;
	localService.sin_addr.s_addr = htonl(INADDR_ANY);
	localService.sin_port = htons(0); 


	if (bind(connectSocket, (SOCKADDR*)&localService, sizeof(localService)) == SOCKET_ERROR)
	{
		printf("bind failed with error : %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;

	}

	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)connectSocket, iocpHandle, key, 0);

	thread t(ConnectThread, iocpHandle);

	DWORD numOfBytes = 0;
	//overlapped 대신에 session
	Session* connectSession = new Session;
	connectSession->type = CONNECT;
	//WSAOVERLAPPED connectOverlapped = {};

	if (lpfnConnectEx(connectSocket, (SOCKADDR*)&service, sizeof(service), nullptr, 0, &numOfBytes, &connectSession->overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("ConnectEx failed with error : %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

	}

	//overlapped 대신에 session
	Session* disconnectSession = new Session;
	disconnectSession->type = DISCONNECT;
	//WSAOVERLAPPED disconnectOverlapped = {};

	if (!lpfnDisconnectEx(connectSocket, &disconnectSession->overlapped, 0, 0))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("DisconnectEx failed with error : %d\n", WSAGetLastError());
			closesocket(connectSocket);
			WSACleanup();
			return 1;
		}

	}


	t.join();


	closesocket(connectSocket);
	WSACleanup();
}