#include "pch.h"
#include "ServerService.h"
#include "Listener.h"
#include "IocpEvent.h"


ServerService::ServerService(wstring ip, u_short port, SessionFactory factory) : Service(ServiceType::SERVER, ip, port, factory)
{
}

bool ServerService::Start()
{
    listener = new Listener;
    return (listener->StartAccept(this));
}