#include "pch.h"
#include "Session.h"
#include "SocketHelper.h"

Session::Session()
{
	socket = SocketHelper::CreateSocket();
}

Session::~Session()
{
	SocketHelper::CloseSocket(socket);
}

void Session::ProcessConnect()
{
	printf("Session::ProcessConnect\n");
}

HANDLE Session::GetHandle()
{
	return (HANDLE)socket;
}

void Session::ObserveIO(IocpEvent* iocpEvent, int numOfBytes)
{
	//Todo
}
