#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"

Service::Service(wstring ip, u_short port)
{
	SocketHelper::StartUp();

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	IN_ADDR address;
	//ip�޾Ƽ� address�� ä���
	InetPton(AF_INET, ip.c_str(), &address);
	sockAddr.sin_addr = address;
	sockAddr.sin_port = htons(port);
}

Service::~Service()
{
	SocketHelper::CleanUp();
}