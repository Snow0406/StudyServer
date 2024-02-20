#include <iostream>
using namespace std;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <vector>

struct Session
{
    SOCKET sock = INVALID_SOCKET;
    char recvBuffer[512] = {};
    int recvLen = 0;
    int sendLen = 0;
};

int main()
{
    printf("============ SERVER ===========\n");

    WORD wVersionRequested; //서버가 사용할 수 있는 Windows 소켓 버전 설정 // NOLINT(clang-diagnostic-invalid-utf8)
    WSAData wsaData; //Windows 소켓 구현에 대한 정보 받기 위해 // NOLINT(clang-diagnostic-invalid-utf8)

    wVersionRequested = MAKEWORD(2, 2); //버전 만들어서 넣어줌 // NOLINT(clang-diagnostic-invalid-utf8)

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

    SOCKADDR_IN service;
    //memset(start address, value, size) 0으로 구조체(service)를 초기화 // NOLINT(clang-diagnostic-invalid-utf8)
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

    printf("listening...");

    while (true)
    {
        SOCKET acceptSocket = INVALID_SOCKET;

        while (true)
        {
            //non-blocking으로 accept 실행
            acceptSocket = accept(listenSocket, nullptr, nullptr);

            if (acceptSocket == INVALID_SOCKET)
            {
                if (WSAGetLastError() == WSAEWOULDBLOCK)
                {
                    continue;
                }

                closesocket(listenSocket);
                WSACleanup();
                return 1;
            }
            else
            {
                printf("Client Connected\n");
                break;
            }
        }

        //WSARecv
        //s : 소켓. 위에 acceptSocket 넣어 주면됨
        //lpBuffers : WSABUF 구조체 배열의대한 포인터
        //dwBufferCount : WSABUF 구조체 갯수
        //lpNumberOfBytesRecvd : 수신 완료 후 받은 바이트에 대한 포인터
        //lpFlages : 플래그
        //lpOverlapped : WSAOVERLAPPED 구조체 대한 포인터
        //lpCompletionRoutine : 수신 완료 후 호출되는 콜백함수

        WSAOVERLAPPED overrlapped = {};
        WSAEVENT wsaEvent = WSACreateEvent(); //비동기 작업을
        overrlapped.hEvent = wsaEvent; //WSAOVERLAPPED 구조체에 있는 hEvent에 객체 연결

        char recvBuffer[512];

        while (true)
        {
            WSABUF wsaBuf; //WSABUF 구조체 선언, WSARecv 함수에 사용될 버퍼를 관리
            wsaBuf.buf = recvBuffer; //버퍼 포인터를 recvBuffer로 설정
            wsaBuf.len = sizeof(recvBuffer); //버퍼 크기 설정

            DWORD recvLen = 0; //수신된 데이터 길이를 저장할 변수
            DWORD flags = 0; //플래그 변수, 현재는 사용하지 않음

            //WSARecv 함수를 이용해 비동기적으로 데이터를 수신 요청
            if (WSARecv(acceptSocket, &wsaBuf, 1, &recvLen, &flags, &overrlapped, nullptr))
            //WSARecv 함수를 아용해 비동기적으로 데이터를 수신 요청
            {
                //Recv 작업이 성공적으로 시작되었으며 나중에 완료될 경우
                if (WSAGetLastError() == WSA_IO_PENDING)
                {
                    //스신 작업이 완료될때까지 이벤트 대기
                    WSAWaitForMultipleEvents(1, &wsaEvent, TRUE, WSA_INFINITE, FALSE);
                    //완료된 비동기 작업을 가져옴. 이 과정에서 실제로 데이터가 수신됨
                    WSAGetOverlappedResult(acceptSocket, &overrlapped, &recvLen, FALSE, &flags);
                }
                else //SOCKET_ERROR인 상태에서 WSA_IO_PENDING이 아니라면 진짜 에러
                {
                    //수신 루프 빠져 나가기
                    break;
                }
            }

            printf("Recv Data : %s\n", recvBuffer);
            
        }
        closesocket(acceptSocket);	//해당 소켓은 닫아주고
        WSACloseEvent(wsaEvent);	// 사용된 이벤트 객체를 닫음
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
