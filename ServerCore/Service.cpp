#include "pch.h"
#include "Service.h"
#include "SocketHelper.h"
#include "IocpCore.h"
#include "Listener.h"

Service::Service(wstring ip, u_short port)
{
	if (!SocketHelper::StartUp())
		return;

	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = AF_INET;

	IN_ADDR address;
	//ip받아서 address에 채운다
	InetPton(AF_INET, ip.c_str(), &address);
	sockAddr.sin_addr = address;
	sockAddr.sin_port = htons(port);

	iocpCore = new IocpCore;
}

Service::~Service()
{
	if (iocpCore != NULL)
	{
		SocketHelper::CleanUp();
		delete iocpCore;
		iocpCore = NULL;
	}
}

bool Service::Start()
{
	listener = new Listener;
	return listener->StartAccept(this);
}
