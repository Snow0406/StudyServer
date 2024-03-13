#include "pch.h"
#include <ServerService.h>
#include <Session.h>
#include <IocpCore.h>

class ServerSession : public Session
{
	//Todo

};

int main()
{
	printf("============= SERVER =============\n");

	Service* service = new ServerService(L"127.0.0.1", 27015, []() {return new ServerSession; });

	if (!service->Start())
	{
		printf("Server Start Error\n");
		return 1;
	}
	thread t([=]() {
		while (true)
		{
			service->GetIocpCore()->ObserverIO();
		}
		});

	t.join();

	delete service;

	return 0;


}