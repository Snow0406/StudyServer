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


    //Event와 Socket을 담을 vector를 할당
    //1(SOCKET) + 1(WSAEVENT)이라 같이 움직인다고 생각하면 됨.
    vector<SOCKET> sockets;
    vector<WSAEVENT> wsaEvents;

    sockets.push_back(listenSocket);

    //WSACreateEvent로 listenSocket이랑 같이 동작할 이벤트 객체 만듬
    WSAEVENT listenEvent = WSACreateEvent();

    //해당 listenEvent를 wsaEvents vector에 등록
    wsaEvents.push_back(listenEvent);

    //소켓과 이벤트를 연동
    //해당 소켓과 이벤트를 1대1로 WSAEventSelect에 넣어줌
    //이벤트 감시할꺼 설정 해줌
    //listenSocket 하는 역할이 accept & 접속 끊겼을떄 처리
    //FD_ACCEPT : 접속한 클라가 있어서 accept함수를 바로 호출 할수 있는 경우를 관찰
    //FD_CLOSE : 상대가 접속 종료 했을 경우 관찰
    if (WSAEventSelect(listenSocket, listenEvent, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR)
    {
        printf("WSAEventSelect Error %d\n", WSAGetLastError());
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }
    //Socket sock;
    //fd_set set;
    //FD_ZERO : 빈 집합을 초기화. ex) FD_ZERO(set); // NOLINT(clang-diagnostic-invalid-utf8)
    //FD_CLR  : 집합에서 소켓을 제거. ex) FD_CLR(sock, &set); // 해당 소켓을 해당 집합에서 제거 // NOLINT(clang-diagnostic-invalid-utf8)
    //FD_ISSET: 해당 소켓이 집합의 맴버인지 확인, set에 들어가 있으면 TRUE를 반환, ex) FD_ISSET(sock, &set); // NOLINT(clang-diagnostic-invalid-utf8)
    //FD_SET  : 소켓을 추가. ex) FD_SET(sock, &set); //해당 set에 sock 추가 // NOLINT(clang-diagnostic-invalid-utf8)

    while (true)
    {
        //WSAWaitForMultipleEvents :여러 이벤트 감시
        //cEvents : 이벤트 갯수
        //lphEvents : 이벤트 배열의 시작 주소
        //fWaitAll : (true)모든 이벤트들을 기다릴것인지, (False)준비 되는 대로 값을 반환할것인지
        //dwTimeout : 이벤트 기다리는 시간
        //fAlertable : 스레드 경구 가능한 대기 상태에 배치되는지 여부를 저장하는 값

        //wsaEvents.size() : 이벤트 갯수 listenEvent + 클라와 연결한 소켓과 1+1인 이벤트를...
        //&wsaEvents[0] : vector 배열 형식이니깐, 해당 배열의 시작 주소.
        //FALSE : 준비되는 족족 반환
        //WSA_INFINITE : 무한으로 기다림
        //FALSE : 값 설정 안함
        DWORD index = WSAWaitForMultipleEvents(wsaEvents.size(), &wsaEvents[0], FALSE, WSA_INFINITE, FALSE);

        if (index == WSA_WAIT_FAILED)
        {
            continue;
        }

        //WSAWaitForMultipleEvents 반환값 == STATUS_WAIT_0  ((DWORD   )0x00000000L) + wsaEvents벡터에서 발생한 이벤트 인덱스 값
        //정확한 인덱스 값을 얻어 내기 위해서 WSA_WAIT_EVENT_0값을 빼줘야 함
        index -= WSA_WAIT_EVENT_0;

        WSANETWORKEVENTS networkEvents;

        //WSAEnumNetworkEvents : 소켓과 이벤트가 어떤 상태인지 확인 하기 위해서
        //이벤트는 발생한거고 어느 index에서 발생한건 아니까
        //[in] s : 소켓
        //[in] hEventObject : 이벤트
        //[out] lpNetworkEvents : 개체 식별하기 위한 핸들
        if (WSAEnumNetworkEvents(sockets[index], wsaEvents[index], &networkEvents) == SOCKET_ERROR)
        {
            continue;
        }
        
#pragma region ACCEPT
        
        //listenSocket 체크
        //해당 소켓이 accept할 준비가 끝났다면
        //networkEvents 이벤트 까서
        //&비트 연산으로 체크
        if (networkEvents.lNetworkEvents & FD_ACCEPT)
        {
            //에러체크
            //비트값을 이용해서 에러 체크
            if (networkEvents.iErrorCode[FD_ACCEPT_BIT] != 0)
            {
                continue;
            }
            //손님을 바로 받음 됨
            SOCKET acceptSocket = accept(listenSocket, NULL, NULL);

            //acceptSocket이 INVALID_SOCKET이 아니라면 정상이니까
            if (acceptSocket != INVALID_SOCKET)
            {
                printf("Client Connected...");
                sockets.push_back(acceptSocket);

                //accpetEvent 대응 할 이벤트 하나 만듦
                WSAEVENT accpetEvent = WSACreateEvent();

                //1 + 1으로 해당 이벤트오 wsaEvnets vector에 추가
                wsaEvents.push_back(accpetEvent);

                //해당 acceptSocket 이벤트를 관찰할수 있게
                //FD_READ : recv, recvfrom 함수 호출 할수 있는 경우 관찰
                //FD_WRITE : send, sendto 함수 호출 할수 있는 경우 관찰
                //FD_CLOSE : 상대가 접속 종료 했을 경우 관찰

                if (WSAEventSelect(acceptSocket, accpetEvent, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
                {
                    printf("WSAEventSelect Error %d\n", WSAGetLastError());
                    closesocket(acceptSocket);
                    closesocket(listenSocket);
                    WSACleanup();
                    return 1;
                }
            }
        }
        
#pragma endregion
#pragma region READ
        
        if (networkEvents.lNetworkEvents & FD_READ)
        {
            if (networkEvents.iErrorCode[FD_READ_BIT] != 0)
            {
                continue;
            }

            //이벤트가 발생한 해당소켓을 참조
            SOCKET& sock = sockets[index];

            //데이터 받을 버퍼
            char recvBuffer[512];
            //준비가 되어 있으니 바로 받음 됨
            int recvLen = recv(sock, recvBuffer, sizeof(recvBuffer), 0);
            //에러 발생시
            if (recvLen == SOCKET_ERROR)
            {
                //WSAEWOULDBLOCK 상태가 아니면 문제가 있는거니까
                if (WSAGetLastError() != WSAEWOULDBLOCK)
                {
                 continue;   
                }
            }
            printf("Recv Data : %s\n", recvBuffer);
            
        }
        
#pragma endregion
        
    }
    closesocket(listenSocket);
    WSACleanup();
    return 0;
}
