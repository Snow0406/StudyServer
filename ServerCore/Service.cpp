#include "pch.h"
#include "Service.h"

Service::Service(wstring ip, u_short port)
{
	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		printf("WSAStartup failed with error\n");
		return;
	}

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	IN_ADDR address;
	//ip받아서 address에 채운다
	InetPton(AF_INET, ip.c_str(), &address);
	sockAddr.sin_addr = address;
	sockAddr.sin_port = htons(port);
}

Service::~Service()
{
	WSACleanup();
}