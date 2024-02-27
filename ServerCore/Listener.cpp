#include "pch.h"
#include "Listener.h"

Listener::Listener()
{
}

Listener::~Listener()
{
}

bool Listener::StartAccept(Service& service)
{
    return false;
}

void Listener::CloseSocket()
{
}
