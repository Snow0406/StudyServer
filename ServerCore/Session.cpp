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
}

HANDLE Session::GetHandle()
{
	return HANDLE();
}

void Session::ObserveIO(IocpEvent* iocpEvent, int numOfBytes)
{
}
