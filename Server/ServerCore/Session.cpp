#include "pch.h"
#include "Session.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"


Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketUtils::CreateSocket();
	if (_socket == INVALID_SOCKET)
		CRASH("CreateSocket");
}

Session::~Session()
{
	::closesocket(_socket);
}

void Session::Send(SendBufferRef sendBuffer)
{
	{
		WRITE_LOCK;

		_sendQueue.push(sendBuffer);
	}

	bool expected = false;

	if (_isSend.compare_exchange_strong(expected, true))
	{
		RegisterSend();
	}
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	wcout << cause << endl;
	if (!IsConnected())
		CRASH("not connected, does not disconnect");

	bool expected = _connected.load();

	if (_connected.compare_exchange_strong(expected, false))
	{
		RegisterDisconnect();
	}
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, int32 numOfBytes)
{
	switch (iocpEvent->_type)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		CRASH("IocpEvent Error");
		break;
	}
}

void Session::ProcessSend(int32 numOfBytes)
{
	_sendEvent._owner = nullptr;
	_sendVector.clear();

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}

	OnSend(numOfBytes);

	{
		WRITE_LOCK;
		if (_sendQueue.empty())
			_isSend.store(false);
		else
			RegisterSend();
	}
}

void Session::ProcessRecv(int32 numOfBytes)
{
	_recvEvent._owner = nullptr;

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}
	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"WriteOverFlow");
		return;
	}

	int32 processLen = OnRecv(_recvBuffer.ReadPos(), numOfBytes);
	if (processLen < 0 || numOfBytes < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"Read Overflow");
		return;
	}

	_recvBuffer.Clean();

	RegisterRecv();
}

void Session::ProcessConnect()
{
	if (IsConnected()) 
	{
		CRASH("DoubleConnect");
		return;
	}

	_connected.store(true);

	OnConnected();

	GSESSIONS->PushSession(GetSessionRef());

	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent._owner = nullptr;

	OnDisconnected();

	GSESSIONS->PopSession(GetSessionRef());
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent._owner = shared_from_this();

#ifdef SINGLE
	{
		WRITE_LOCK;
		SendBufferRef ref = _sendQueue.front();

		_sendVector.push_back(ref);
		_sendQueue.pop();
	}

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_sendVector[0]->Buffer());
	wsaBuf.len = _sendVector[0]->WriteSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == WSASend(_socket, &wsaBuf, 1, &numOfBytes, flags, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			CRASH(errorCode);
			_sendEvent._owner = nullptr;

			// Send Vector�� �ִ� ������ �ٽ� �Űܾ� �ұ�?

			_sendVector.clear();
			_isSend.store(false);
		}
	}
#else
	{
		WRITE_LOCK;
		while (!_sendQueue.empty())
		{
			SendBufferRef ref = _sendQueue.front();

			_sendVector.push_back(ref);
			_sendQueue.pop();
		}
	}

	vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendVector.size());

	for (SendBufferRef ref : _sendVector)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(ref->Buffer());
		wsaBuf.len = ref->WriteSize();
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), &numOfBytes, 0, &_sendEvent, nullptr))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			CRASH(errorCode);
			_sendEvent._owner = nullptr;

			// Send Vector

			_sendVector.clear();
			_isSend.store(false);

			Disconnect(L"Send Disconnect");
		}
	}

#endif // SINGLE
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent._owner = shared_from_this(); // ADD_REF

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;

	if (::WSARecv(_socket, &wsaBuf, 1, &numOfBytes, &flags, &_recvEvent, nullptr) == SOCKET_ERROR)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			CRASH(errorCode);
			_recvEvent._owner = nullptr;

			Disconnect(L"Recv Disconnect");
		}
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (SocketUtils::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketUtils::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent._owner = shared_from_this();

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetNetAddress().GetSockAddr();
	if (SOCKET_ERROR == SocketUtils::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), /*접속 후 전송할 데이터 있으면 사용(buf)*/nullptr, /*접속 후 전송할 데이터 있으면 사용(len)*/0, &numOfBytes, &_connectEvent))
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent._owner = nullptr; // RELEASE_REF
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent._owner = shared_from_this();

	if (SocketUtils::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0) == false)
	{
		int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			CRASH("Already Disconnect");
			_disconnectEvent._owner = nullptr;
			return false;
		}
	}

	return true;
}

PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}

int32 PacketSession::OnRecv(BYTE* buffer, int32 len)
{
	int32 processLen = 0;
	while (true)
	{
		int32 dataSize = len - processLen;

		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader* head = reinterpret_cast<PacketHeader*>(&buffer[processLen]);
		if (head->size > dataSize)
			break;

		OnRecvPacket(&buffer[processLen], head->size);

		processLen += head->size;
	}

	return processLen;
}
