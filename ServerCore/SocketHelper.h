#pragma once
class SocketHelper
{
public:
	static LPFN_ACCEPTEX AcceptEx;
public:
	static bool StartUp();
	static void CleanUp();
public:
	static bool SetIOControl(SOCKET socket, GUID guid, LPVOID* func);
	static SOCKET CreateSocket();
public:
	//Socket Option
	static bool SetReuseAddress(SOCKET socket, bool enable);
	static bool SetLinger(SOCKET socket, u_short onOff, u_short time);
	//listenSocket Á¤º¸, acceptSocket ¾÷µ«
	static bool SetUpdateAcceptSocket(SOCKET acceptSocket, SOCKET listenSocket);
public:
	static bool Bind(SOCKET socket, SOCKADDR_IN sockAddr);
	static bool Listen(SOCKET socket, int backlog = SOMAXCONN);
	static void CloseSocket(SOCKET& socket);
};

template<typename T>
static inline bool SetSocketOpt(SOCKET socket, int level, int optName, T optVal)
{
	return setsockopt(socket, level, optName, (char*)&optVal, sizeof(T)) != SOCKET_ERROR;
}