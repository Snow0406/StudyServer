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

	//접속할 소켓을 담을 구조체 // NOLINT(clang-diagnostic-invalid-utf8)
	vector<Session> sessions;

	fd_set reads;
	fd_set writes;

	//Socket sock;
	//fd_set set;
	//FD_ZERO : 빈 집합을 초기화. ex) FD_ZERO(set); // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_CLR  : 집합에서 소켓을 제거. ex) FD_CLR(sock, &set); // 해당 소켓을 해당 집합에서 제거 // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_ISSET: 해당 소켓이 집합의 맴버인지 확인, set에 들어가 있으면 TRUE를 반환, ex) FD_ISSET(sock, &set); // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_SET  : 소켓을 추가. ex) FD_SET(sock, &set); //해당 set에 sock 추가 // NOLINT(clang-diagnostic-invalid-utf8)

	while (true)
	{
		FD_ZERO(&reads); //reads를 초기화 // NOLINT(clang-diagnostic-invalid-utf8)
		FD_ZERO(&writes);

		for (Session& session : sessions)
		{
			if (session.recvLen <= session.sendLen)
			{
				FD_SET(session.sock, &reads);
			}
			else FD_SET(session.sock, &writes);

			FD_SET(listenSocket, &reads);
				FD_SET(listenSocket, &writes);
		}

		FD_SET(listenSocket, &reads); //listen 소켓도 reads에 등록 -> 클라에서도 connect  // NOLINT(clang-diagnostic-invalid-utf8)


		//등록된 애들중에 send를 했거나 새로운 클라가 connect를 했을떄 // NOLINT(clang-diagnostic-invalid-utf8)
		//값이 있으면 뱉어냄 -> reads에 등록된 애들중에 이벤트가 있으면 // NOLINT(clang-diagnostic-invalid-utf8)
		if (select(0, &reads, &writes, nullptr, nullptr) == SOCKET_ERROR)
		{
			printf("select Error %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		if (FD_ISSET(listenSocket, &reads)) //현재 reads에 있음? // NOLINT(clang-diagnostic-invalid-utf8)
		{
			SOCKET acceptSocket = accept(listenSocket, nullptr, nullptr);
			Session session;
			session.sock = acceptSocket;

			sessions.push_back(session);
			printf("Client Connected.\n");
		}

		for (Session& session : sessions)
		{
			//해당 소켓이 무언가를 보냈다면 reads에 등록 되어 있을꺼고
			//FE_ISSET은 해당 소켓이 등록되어 있는지 없는지만 체크
			if (FD_ISSET(session.sock, &reads))
			{

				//Non blocking recv <- 여기 까지 왔다는건 select 넘어와서 reads에 등록되어 있으니
				//준비가 되어 있는 상태
				int recvLen = recv(session.sock, session.recvBuffer, sizeof(session.recvBuffer), 0);

				//보냈긴 했는데 값이 없으면
				if (recvLen <= 0)
				{
					//해당 소켓을 날림
					//sockets.erase(remove(sockets.begin(), sockets.end(), sock), sockets.end());
					continue;
				}

				session.recvLen = recvLen;
				printf("Recv Data : %s\n", session.recvBuffer);
			}

			//write에 해당 socket이 있다면
			if (FD_ISSET(session.sock, &writes))
			{
				char sendBuffer[] = "hello this is server !!!!";

				int sendLen = send(session.sock, sendBuffer, sizeof(sendBuffer), 0);

				if (sendLen == SOCKET_ERROR)
				{
					//sockets.erase(remove(sockets.begin(), sockets.end(), sock), sockets.end());
					continue;
				}

				session.sendLen = sendLen;
				if (session.sendLen == session.recvLen)
				{
					session.sendLen = 0;
					session.recvLen = 0;
				}

				printf("Send Buffer Length : %d byte \n", sendLen);
			}
		}
	}


	closesocket(listenSocket);
	WSACleanup();
	return 0;
}
