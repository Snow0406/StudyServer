#include "pch.h"
#include "Listener.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Session.h"

Listener::~Listener()
{
	CloseSocket();
}

bool Listener::StartAccept(Service* service)
{
	socket = SocketHelper::CreateSocket();
	if (socket == INVALID_SOCKET)
		return false;

	if (!SocketHelper::SetReuseAddress(socket, true))
		return false;
	
	if (!SocketHelper::SetLinger(socket, 0, 0))
		return false;

	ULONG_PTR key = 0;
	service->GetIocpCore()->Register((HANDLE)socket, key);

	if (!SocketHelper::Bind(socket, service->GetSockAddr()))
		return false;
	
	if (!SocketHelper::Listen(socket))
		return false;

	printf("Listening...");

	SOCKET acceptSocket = SocketHelper::CreateSocket();
	if (acceptSocket == INVALID_SOCKET)
		return false;

	Session* session = new Session;
	session->socket = acceptSocket;

	DWORD dwBytes = 0;

	if (!SocketHelper::AcceptEx(socket, acceptSocket, lpOutputBuf, 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &dwBytes, &overlapped))
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
		{
			return false;
		}
	}


    return true;
}

void Listener::CloseSocket()
{
	SocketHelper::CloseSocket(socket);
}
