#pragma once
#include "Listener.h"

using SessionFunction = std::function<SessionRef()>;

class Service
{
public:
	Service(NetAddress addr, int32 socketCount, SessionFunction function);
	virtual ~Service();

	virtual bool Start() abstract;
	NetAddress GetNetAdress() { return _addr; }
	IocpCoreRef GetIocpCore() { return _iocpCore; }

protected:
	IocpCoreRef _iocpCore;
	NetAddress _addr;
	SessionFunction _sessionFunction;
	int32 _maxSessionCount;
};

class ClientService : public Service
{
	using Super = Service;
public:
	ClientService(NetAddress addr, int32 socketCount, SessionFunction function);
	~ClientService();

	virtual bool Start() override;
};

class ServerService : public Service
{
	using Super = Service;
public:
	ServerService(NetAddress addr, int32 socketCount, SessionFunction function);
	~ServerService();

	virtual bool Start() override;
	
private:
	ListenerRef _listener;
};