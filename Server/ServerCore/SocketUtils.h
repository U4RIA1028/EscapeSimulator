#pragma once
#include "NetAddress.h"

/*----------------
	SocketUtils
-----------------*/

class SocketUtils
{
public:
	static LPFN_CONNECTEX		ConnectEx;
	static LPFN_DISCONNECTEX	DisconnectEx;
	static LPFN_ACCEPTEX		AcceptEx;

public:
	static void Init();
	static void Clear();

	// Ex 함수들 초기화 하는 함수
	static bool BindWindowsFunction(SOCKET socket, GUID guid, LPVOID* fn);
	static SOCKET CreateSocket();

	// Close 되었을 때, 소켓 버퍼에 남아있는 데이터를 어떻게 할 것인지 결정
	static bool SetLinger(SOCKET socket, uint16 onoff, uint16 linger);
	
	// 커널이 bind 정보를 들고 있을 때, 재 사용이 가능하도록 한다.
	static bool SetReuseAddress(SOCKET socket, bool flag);
	
	// Send, Recv 버퍼 크기 지정
	static bool SetRecvBufferSize(SOCKET socket, int32 size);
	static bool SetSendBufferSize(SOCKET socket, int32 size);

	// Nagle 알고리즘 -> 작은 데이터들을 모아서 한번에 보내는 알고리즘( 껏다 켜기 )
	static bool SetTcpNoDelay(SOCKET socket, bool flag);

	// ListenSocket의 특성을 ClientSocket에 그대로 적용
	static bool SetUpdateAcceptSocket(SOCKET socket, SOCKET listenSocket);
	
	static bool Bind(SOCKET socket, NetAddress netAddr);
	static bool Bind(SOCKET socket, SOCKADDR_IN netAddr);
	static bool BindAnyAddress(SOCKET socket, uint16 port);
	static bool Listen(SOCKET socket, int32 backlog = SOMAXCONN);
	static void Close(SOCKET& socket);
	static string GetHostDomainbyIp(string domain);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int32 level, int32 optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<char*>(&optVal), sizeof(T));
}