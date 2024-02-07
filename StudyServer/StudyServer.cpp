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
    WORD wVersionRequested; //������ ����� �� �ִ� Windows ���� ���� ����
    WSAData wsaData; //Windows ���� ������ ���� ���� �ޱ� ����

    wVersionRequested = MAKEWORD(2, 2); //���� ���� �־���

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
    SOCKADDR_IN service; //memset(start address, value, size) 0���� ����ü(service)�� �ʱ�ȭ
    memset(&service, 0, sizeof(service));

    service.sin_family = AddressFamily; // Address Family : IPv4
    inet_pton(AF_INET, IPAddress, &service.sin_addr);
    service.sin_port = htons(PORT);

    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) //listenSocket(��ȭ��)�� SOCKADDR(�������)�� ���
    {
        printf("bind function failed %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, 10) == SOCKET_ERROR) //listen �� ��� ����Ҳ���
    {
        printf("listen function failed %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        printf("listening...\n");
        SOCKET acceptSocket = accept(listenSocket, NULL, NULL); //Ŭ���̾�Ʈ ���ӽ� Ŭ���̾�Ʈ�� ������ ������ ��ȯ, Ŭ���̾�Ʈ�� �����Ҷ����� ��� (�����Լ�)
        
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
