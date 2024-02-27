#pragma once
#include "CorePch.h"
class Listener
{
private:
	SOCKET socket = INVALID_SOCKET;
public:
	Listener();
	~Listener();
public:
	bool StartAccept(class Service& service);
	void CloseSocket();
};

