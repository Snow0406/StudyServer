#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#define AddressFamily AF_INET
#define IPAddress "127.0.0.1"
#define PORT 7777

int main()
{
    WORD wVersionRequested; //서버가 사용할 수 있는 Windows 소켓 버전 설정
    WSAData wsaData; //Windows 소켓 구현에 대한 정보 받기 위해

    wVersionRequested = MAKEWORD(2, 2); //버전 만들어서 넣어줌

    if (WSAStartup(wVersionRequested, &wsaData) != 0) {
        printf("WSAStartup Failed with error\n");
        return 1;
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket function failed %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    printf("socket function succeeded\n");

    //Todo
    SOCKADDR_IN service; //memset(start address, value, size) 0으로 구조체(service)를 초기화
    memset(&service, 0, sizeof(service));

    service.sin_family = AddressFamily; // Address Family : IPv4
    inet_pton(AF_INET, IPAddress, &service.sin_addr);
    service.sin_port = htons(PORT);

    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) //listenSocket(전화기)에 SOCKADDR(등록정보)를 등록
    {
        printf("bind function failed %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, 10) == SOCKET_ERROR) //listen 에 몇명 대기할껀지
    {
        printf("listen function failed %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        printf("listening...\n");
        SOCKET acceptSocket = accept(listenSocket, NULL, NULL); //클라이언트 접속시 클라이언트랑 연락할 소켓을 반환, 클라이언트가 접속할때까지 대기 (동기함수)
        
        if (acceptSocket == INVALID_SOCKET)
        {
            printf("accept function failed %d\n", WSAGetLastError());
            closesocket(acceptSocket);
            WSACleanup();
            return 1;
        }

        printf("Client connected\n");
    }
    
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
