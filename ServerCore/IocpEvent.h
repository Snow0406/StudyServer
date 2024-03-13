#pragma once

enum class EventType : u_char
{
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND,
};

class Session;

class IocpEvent : public OVERLAPPED
{
public:
	EventType eventType;
	class IocpObj* iocpObj;
public:
	IocpEvent(EventType type);
public:
	void Init();
};


class AcceptEvent : public IocpEvent
{
public:
	Session* session = nullptr;
public:
	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

//Recv Ãß°¡
class RecvEvent : public IocpEvent
{
public:
	RecvEvent() : IocpEvent(EventType::RECV) {}

};