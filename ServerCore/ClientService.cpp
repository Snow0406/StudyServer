#include "pch.h"
#include "ClientService.h"

ClientService::ClientService(wstring ip, u_short port, SessionFactory factory) : Service(ServiceType::CLIENT, ip, port, factory)
{
}

bool ClientService::Start()
{
    Session* session = CreateSession();
    return session->Connect();
}
