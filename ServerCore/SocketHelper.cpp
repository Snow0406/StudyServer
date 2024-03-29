#include "pch.h"
#include "SocketHelper.h"

LPFN_ACCEPTEX SocketHelper::AcceptEx = NULL;

bool SocketHelper::StartUp()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        return false;
    // != 0 == error : return false; not error: return true;

    SOCKET tempSocklet = CreateSocket();
    SetIOControl(tempSocklet, WSAID_ACCEPTEX, (LPVOID*)&AcceptEx);
    CloseSocket(tempSocklet);
    return true;
}

void SocketHelper::CleanUp()
{
    WSACleanup();
}

bool SocketHelper::SetIOControl(SOCKET socket, GUID guid, LPVOID* func)
{
    DWORD dwBytes = 0;
    return WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid), func, sizeof(*func), &dwBytes, NULL, NULL);
}

SOCKET SocketHelper::CreateSocket()
{
    return WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketHelper::SetReuseAddress(SOCKET socket, bool enable)
{
    return SetSocketOpt(socket, SOL_SOCKET, SO_REUSEADDR, enable);
}

bool SocketHelper::SetLinger(SOCKET socket, u_short onOff, u_short time)
{
    LINGER linger;
    linger.l_linger = time;
    linger.l_onoff = onOff;
    return SetSocketOpt(socket, SOL_SOCKET, SO_LINGER, linger);
}

bool SocketHelper::SetUpdateAcceptSocket(SOCKET acceptSocket, SOCKET listenSocket)
{
    return SetSocketOpt(acceptSocket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketHelper::Bind(SOCKET socket, SOCKADDR_IN sockAddr)
{
    return bind(socket, (SOCKADDR*)&sockAddr, sizeof(sockAddr)) != SOCKET_ERROR;
}

bool SocketHelper::Listen(SOCKET socket, int backlog)
{
    return listen(socket, backlog) != SOCKET_ERROR;
}

void SocketHelper::CloseSocket(SOCKET& socket)
{
    if (socket != INVALID_SOCKET)
    {
        closesocket(socket);
    }
    socket = INVALID_SOCKET;
}
