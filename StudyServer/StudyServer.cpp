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

	WORD wVersionRequested; //������ ����� �� �ִ� Windows ���� ���� ���� // NOLINT(clang-diagnostic-invalid-utf8)
	WSAData wsaData; //Windows ���� ������ ���� ���� �ޱ� ���� // NOLINT(clang-diagnostic-invalid-utf8)

	wVersionRequested = MAKEWORD(2, 2); //���� ���� �־��� // NOLINT(clang-diagnostic-invalid-utf8)

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
	//memset(start address, value, size) 0���� ����ü(service)�� �ʱ�ȭ // NOLINT(clang-diagnostic-invalid-utf8)
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

	//������ ������ ���� ����ü // NOLINT(clang-diagnostic-invalid-utf8)
	vector<Session> sessions;

	fd_set reads;
	fd_set writes;

	//Socket sock;
	//fd_set set;
	//FD_ZERO : �� ������ �ʱ�ȭ. ex) FD_ZERO(set); // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_CLR  : ���տ��� ������ ����. ex) FD_CLR(sock, &set); // �ش� ������ �ش� ���տ��� ���� // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_ISSET: �ش� ������ ������ �ɹ����� Ȯ��, set�� �� ������ TRUE�� ��ȯ, ex) FD_ISSET(sock, &set); // NOLINT(clang-diagnostic-invalid-utf8)
	//FD_SET  : ������ �߰�. ex) FD_SET(sock, &set); //�ش� set�� sock �߰� // NOLINT(clang-diagnostic-invalid-utf8)

	while (true)
	{
		FD_ZERO(&reads); //reads�� �ʱ�ȭ // NOLINT(clang-diagnostic-invalid-utf8)
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

		FD_SET(listenSocket, &reads); //listen ���ϵ� reads�� ��� -> Ŭ�󿡼��� connect  // NOLINT(clang-diagnostic-invalid-utf8)


		//��ϵ� �ֵ��߿� send�� �߰ų� ���ο� Ŭ�� connect�� ������ // NOLINT(clang-diagnostic-invalid-utf8)
		//���� ������ �� -> reads�� ��ϵ� �ֵ��߿� �̺�Ʈ�� ������ // NOLINT(clang-diagnostic-invalid-utf8)
		if (select(0, &reads, &writes, nullptr, nullptr) == SOCKET_ERROR)
		{
			printf("select Error %d\n", WSAGetLastError());
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		if (FD_ISSET(listenSocket, &reads)) //���� reads�� ����? // NOLINT(clang-diagnostic-invalid-utf8)
		{
			SOCKET acceptSocket = accept(listenSocket, nullptr, nullptr);
			Session session;
			session.sock = acceptSocket;

			sessions.push_back(session);
			printf("Client Connected.\n");
		}

		for (Session& session : sessions)
		{
			//�ش� ������ ���𰡸� ���´ٸ� reads�� ��� �Ǿ� ��������
			//FE_ISSET�� �ش� ������ ��ϵǾ� �ִ��� �������� üũ
			if (FD_ISSET(session.sock, &reads))
			{

				//Non blocking recv <- ���� ���� �Դٴ°� select �Ѿ�ͼ� reads�� ��ϵǾ� ������
				//�غ� �Ǿ� �ִ� ����
				int recvLen = recv(session.sock, session.recvBuffer, sizeof(session.recvBuffer), 0);

				//���±� �ߴµ� ���� ������
				if (recvLen <= 0)
				{
					//�ش� ������ ����
					//sockets.erase(remove(sockets.begin(), sockets.end(), sock), sockets.end());
					continue;
				}

				session.recvLen = recvLen;
				printf("Recv Data : %s\n", session.recvBuffer);
			}

			//write�� �ش� socket�� �ִٸ�
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
