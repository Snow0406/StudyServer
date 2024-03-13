#pragma once
#include "Service.h"
class ServerService : public Service
{
private:
	class Listener* listener = nullptr;
public:
	ServerService(wstring ip, u_short port, SessionFactory factory);
	virtual ~ServerService() {}
public:
	virtual bool Start() override;

};