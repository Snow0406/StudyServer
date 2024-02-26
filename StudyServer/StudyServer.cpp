#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>	
#include <WS2tcpip.h> 

#include <thread> 
#include <MSWSock.h> // 추가

//비동기 I/O 작업을 처리하는 스레드 함수
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

	WORD wVersionRequested;
	WSAData wsaData;
	
	wVersionRequested = MAKEWORD(2, 2);

	if (WSAStartup(wVersionRequested, &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return 1;
	}

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
							  
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket function failed with error %d\n", WSAGetLastError());
		WSACleanup();
		return 1;
	}

	SOCKADDR_IN service;
	memset(&service, 0, sizeof(service));
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons(27015);

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

	//AcceptEX 함수포인터 로드
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

	//클라이언트 연결 수락할 소켓을 생성
	SOCKET acceptSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (acceptSocket == INVALID_SOCKET)
	{								   
		printf("Accept Socket failed with error : %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;

	}
	
	//IOCP를 생성
	HANDLE iocpHandle = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	//비동기 I/O 작업 처리를 위한 스레드 시작
	thread t(AcceptThread, iocpHandle);

	//listenSocket을 IOCP에 연결
	ULONG_PTR key = 0;
	CreateIoCompletionPort((HANDLE)listenSocket, iocpHandle, key, 0);

	char lpOutputBuf[1024];
	//WSAOVERLAPPED overlapped;
	//memset(&overlapped, 0, sizeof(overlapped));
	WSAOVERLAPPED overlapped = {};

	//AcceptEX를 호출하여 비동기적으로 클라이언트 연결 수락
	if (!lpfnAcceptEx(listenSocket, acceptSocket, lpOutputBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &overlapped))
	{
		//에러 코드가 PENDING이 아니라면
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			printf("AcceptEx failed with error : %d\n", WSAGetLastError());
			closesocket(acceptSocket);
			closesocket(listenSocket);
			WSACleanup();
			return 1;

		}
	}

	//스레드 종료될때까지 기다리기
	t.join();

	closesocket(listenSocket);
	WSACleanup();

	return 0;

  
}