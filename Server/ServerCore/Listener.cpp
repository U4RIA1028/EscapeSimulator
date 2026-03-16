#include "pch.h"
#include "Listener.h"

Listener::Listener(NetAddress addr, int32 max) : _addr(addr), _socketCountMax(max)
{
	_listenSocket = SocketUtils::CreateSocket();

	if (_listenSocket == INVALID_SOCKET)
		CRASH("Listen Socket Create");

	if (false == SocketUtils::SetReuseAddress(_listenSocket, true))
		CRASH("SetReUse Error");

	if (false == SocketUtils::SetLinger(_listenSocket, 0, 0))
		CRASH("SetLinger Error");

	if (false == SocketUtils::Bind(_listenSocket, _addr))
		CRASH("Bind Error");

	if (false == SocketUtils::Listen(_listenSocket))
		CRASH("Listen Error");
}

Listener::~Listener()
{
	SocketUtils::Close(_listenSocket);

	for (AcceptEvent* acceptEvent : _acceptEvents)
	{
		delete acceptEvent;
	}
}

bool Listener::StartAccept()
{
	if (GetIocpCore()->Register(shared_from_this()) == false)
		return false;

	for (int i = 0; i < _socketCountMax; i++)
	{
		AcceptEvent* acceptEvent = new AcceptEvent;
		acceptEvent->_owner = shared_from_this();
		_acceptEvents.push_back(acceptEvent);
		RegisterAccept(acceptEvent);
	}

	return true;
}

void Listener::RegisterAccept(AcceptEvent* event)
{
	SessionRef session = _sessioFunction();
	GetIocpCore()->Register(session);

	event->sessionRef = session;

	DWORD bytesReceived = 0;
	if (SOCKET_ERROR == SocketUtils::AcceptEx(_listenSocket, session->GetSocket(), session->_recvBuffer.WritePos(), 0, sizeof(SOCKADDR_IN) + 16, sizeof(SOCKADDR_IN) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(event)))
	{
		int32 errCode = GetLastError();
		if (errCode != WSA_IO_PENDING)
		{
			RegisterAccept(event);
			
			// TODO
		}
	}
}

void Listener::ProcessAccept(AcceptEvent* event)
{
	SessionRef session = event->sessionRef;

	if (false == SocketUtils::SetUpdateAcceptSocket(session->GetSocket(), _listenSocket))
	{
		RegisterAccept(event);
		return;
	}

	SOCKADDR_IN sockAddress;
	int32 size = sizeof(sockAddress);
	if (SOCKET_ERROR == ::getpeername(session->GetSocket(), reinterpret_cast<SOCKADDR*>(&sockAddress), &size))
	{
		RegisterAccept(event);
		return;
	}

	session->SetNetAddress(NetAddress(sockAddress));
	session->ProcessConnect();

	RegisterAccept(event);
}

HANDLE Listener::GetHandle()
{
	return reinterpret_cast<HANDLE>(_listenSocket);
}

void Listener::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	if (iocpEvent->_type != EventType::Accept)
		CRASH("Not AcceptType");

	ProcessAccept(static_cast<AcceptEvent*>(iocpEvent));
}
