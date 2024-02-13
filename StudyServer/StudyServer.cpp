#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

int main()
{
    printf("============ SERVER ===========\n");

    WORD wVersionRequested; //������ ����� �� �ִ� Windows ���� ���� ����
    WSAData wsaData; //Windows ���� ������ ���� ���� �ޱ� ����

    wVersionRequested = MAKEWORD(2, 2); //���� ���� �־���

    if (WSAStartup(wVersionRequested, &wsaData) != 0)
    {
        printf("WSAStartup Failed with error\n");
        return 1;
    }

    //TCP
    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (listenSocket == INVALID_SOCKET)
    {
        printf("socket function failed %d\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    u_long iMode = 1;
    if (ioctlsocket(listenSocket, FIONBIO, &iMode) == INVALID_SOCKET)
    {
        printf("ioctlsocket failed %d\n", WSAGetLastError());
        closesocket((listenSocket));
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN service; //memset(start address, value, size) 0���� ����ü(service)�� �ʱ�ȭ
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET; // Address Family : IPv4
    service.sin_addr.s_addr = htonl(INADDR_ANY);
    service.sin_port = htons(7777);

    if (bind(listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
    {
        printf("bind failed with error %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, 10) == SOCKET_ERROR)
    {
        printf("listen failed %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    while (true)
    {
        system("cls");
        printf("listening...");

        SOCKET acceptSocket = accept(listenSocket, NULL, NULL);
        if (acceptSocket == INVALID_SOCKET)
        {
            if (WSAGetLastError() == WSAEWOULDBLOCK)
            {
                continue;
            }

            break;
        }
        printf("Client Connected.\n");

        while (true)
        {
            char recvBuffer[512];
            int recvLen = recv(acceptSocket, recvBuffer, sizeof(recvBuffer), 0);

            if (recvLen == SOCKET_ERROR)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    continue;
                }
                break;
            } else if (recvLen == 0)
            {
                break;
            }
            printf("Recv Data : %s\n", recvBuffer);
        }
    }

    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
