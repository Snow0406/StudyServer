#pragma once

enum class EventType : u_char {
	CONNECT,
	DISCONNECT,
	ACCEPT,
	RECV,
	SEND

};

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
class Session;
class AcceptEvent : public IocpEvent {
public:
	Session* session = NULL;
public:

	AcceptEvent() : IocpEvent(EventType::ACCEPT) {}
};

