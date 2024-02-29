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
