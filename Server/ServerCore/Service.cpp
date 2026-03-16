#include "pch.h"
#include "Service.h"
#include "Listener.h"

Service::Service(NetAddress addr, int32 socketCount, SessionFunction function)
	: _addr(addr), _maxSessionCount(socketCount), _sessionFunction(function)
{
	_iocpCore = make_shared<IocpCore>();
}

Service::~Service()
{

}

ClientService::ClientService(NetAddress addr, int32 socketCount, SessionFunction function) : Super(addr, socketCount, function)
{
}

ClientService::~ClientService()
{
}

bool ClientService::Start()
{
	if (_sessionFunction == nullptr)
		return false;

	for (int i = 0; i < _maxSessionCount; i++)
	{
		SessionRef session = _sessionFunction();
		session->SetNetAddress(_addr);
		if (GetIocpCore()->Register(session) == false)
			CRASH("Can't Register");

		if (session->Connect() == false)
			return false;
	}
	return true;
}

ServerService::ServerService(NetAddress addr, int32 socketCount, SessionFunction function) : Super(addr, socketCount, function)
{
	
}

ServerService::~ServerService()
{

}

bool ServerService::Start()
{
	if (_sessionFunction == nullptr)
		return false;

	_listener = make_shared<Listener>(_addr, _maxSessionCount);

	if (_listener == nullptr)
		return false;

	_listener->SetIocpCore(_iocpCore);
	_listener->SetSessionFunction(_sessionFunction);

	if (_listener->StartAccept() == false)
		return false;
	
	return true;
}
