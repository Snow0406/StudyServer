#pragma once
#include "Service.h"
class ClientService : public Service
{
public:
	ClientService(wstring ip, u_short port, SessionFactory factory);
	virtual ~ClientService() {}
public:
	virtual bool Start() override;
};

