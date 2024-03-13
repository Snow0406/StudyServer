#pragma once
#include <functional>

enum class ServiceType : u_char
{
	SERVER,
	CLIENT,
};


class IocpCore;
class Session;

using SessionFactory = function<Session* (void)>;


class Service
{
private:
	ServiceType serviceType;
	SOCKADDR_IN sockAddr = {};
	IocpCore* iocpCore = nullptr;
protected:
	shared_mutex rwLock;
	set<Session*> sessions;
	int sessionCount = 0;
	SessionFactory sessionFactory;
public:
	Service(ServiceType type, wstring ip, u_short port, SessionFactory factory);
	virtual ~Service();
public:
	ServiceType GetServiceType() const { return serviceType; }
	SOCKADDR_IN& GetSockAddr() { return sockAddr; }
	IocpCore* GetIocpCore() { return iocpCore; }
public:
	void SetSessionFactory(SessionFactory func) { sessionFactory = func; }
	Session* CreateSession();
	void AddSession(Session* session);
	void RemoveSession(Session* session);
	int GetSessionCount() const { return sessionCount; }
public:
	virtual bool Start() abstract;

};