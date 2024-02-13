#include <iostream>
#include <list>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <Windows.h>

int main()
{
    Sleep(1000);
    
    printf("============ CLIENT ===========\n");

    WORD wVersionRequested;
    WSAData wsaData;

    wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup failed\n");
        return 1;
    }

    //DEGRAM == UDP
    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (connectSocket == INVALID_SOCKET)
    {
        printf("socket function failed %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN service; //¼­¹öÀÇ SOCKADDR_IN
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(7777);

    if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("connect failed %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    printf("connected.");

    while (true)
    {
        char sendBuffer[] = "Hello This is Client hahahaha !";
        if (send(connectSocket, sendBuffer, sizeof(sendBuffer), 0) == SOCKET_ERROR)
        {
            printf("Send Failed %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
        
        Sleep(1000);
    }
    closesocket(connectSocket);
    WSACleanup();
}
