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

    u_long iMode = 1;
    if (ioctlsocket(connectSocket, FIONBIO, &iMode) == INVALID_SOCKET)
    {
        printf("ioctlsocket function failed %d\n", WSAGetLastError());
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN service; //������ SOCKADDR_IN
    memset(&service, 0, sizeof(service));
    service.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &service.sin_addr);
    service.sin_port = htons(7777);

    while (true)
    {
        if (connect(connectSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
        {
            //non-blocking ���� �õ� �� �߻��ϴ� �Ϲ����� ���� ó��
            if (WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEALREADY)
            {
                continue;
            }

            //������ �Ϸ�Ǿ����� Ȯ��
            if (WSAGetLastError() == WSAEISCONN)
            {
                break;
            }

            //���� ���� �� ���� �޼��� ���
            printf("connect failed with error %d\n", WSAGetLastError());
            closesocket(connectSocket);
            WSACleanup();
            return 1;
        }
    }
    printf("connected.\n");
    char sendBuffer[] = "Hello This is Client hahahaha !";

    //������ ������ �غ�
    WSAEVENT wsaEvent = WSACreateEvent();	//winsock �̺�Ʈ ��ü�� ����
    WSAOVERLAPPED overlapped = {};			//�񵿱� I/O �۾��� ���� ����ü �ʱ�ȭ
    overlapped.hEvent = wsaEvent;			//overlapped ����ü�� �̺�Ʈ ��ü�� ��

    while (true)
    {
        WSABUF wsaBuf;						//WSABUF ����ü ����. winsock �Լ��� ���� ���۸� ����
        wsaBuf.buf = sendBuffer;			//���� �����͸� sendBuffer�� ����
        wsaBuf.len = sizeof(sendBuffer);	//������ ���� ����

        DWORD sendLen = 0;					//���۵� �������� ���̸� ������ ����
        DWORD flags = 0;					//���� �� ����� �߰� �ɼ� �÷���. ����� ��� ����

        //WSASend �Լ��� ����Ͽ� �����͸� �񵿱������� ����
        if (WSASend(connectSocket, &wsaBuf, 1, &sendLen, flags, &overlapped, nullptr) == SOCKET_ERROR)
        {
            //WSA_IO_PENDING ������ �񵿱� �۾��� ��� ������ �ǹ�
            if (WSAGetLastError() == WSA_IO_PENDING)
            {
                //�񵿱� �۾��� �Ϸ�� ������ ���
                WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
                //�񵿱� �۾��� ����� ������. ���⼭ ������ �����Ͱ� ����
                WSAGetOverlappedResult(connectSocket, &overlapped, &sendLen, FALSE, &flags);
            }
            else //��¥ ����
            {
                break;
            }
        }
        printf("Send Buffer Lebgth : %d bytes\n", sendLen);

        Sleep(1000);
    }

    WSACloseEvent(wsaEvent);
    closesocket(connectSocket);
    WSACleanup();
}
