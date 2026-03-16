#include "pch.h"
#include <iostream>
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "GameSession.h"
#include "GameSessionManager.h"
#include "RoomManager.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerJob(ServerServiceRef& service)
{
	while (true)
	{
		//LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		// 네트워크 입출력 처리 -> 인게임 로직까지 (패킷 핸들러에 의해)
		service->GetIocpCore()->Dispatch(10);

		GThreadManager->DoGlobalQueue();

		// 예약된 일감 처리
		GThreadManager->DoGlobalTimer();
	}
}

int main()
{
	// CALL $(SolutionDir)PacketGenerator\bin\Debug\net6.0\PacketGenerator.exe

	ServerPacketHandler::Init();

	SocketUtils::Init();

	//ServerServiceRef service = make_shared<ServerService>(NetAddress(L"192.168.35.175", 4000), 10, std::function<SessionRef()>(make_shared<GameSession>));
	ServerServiceRef service = make_shared<ServerService>(NetAddress(L"127.0.0.1", 4000), 10, std::function<SessionRef()>(make_shared<GameSession>));

	ASSERT_CRASH(service->Start());

	for (int32 i = 0; i < 5; i++)
	{
		GThreadManager->Launch([&service]()
			{
				DoWorkerJob(service);
			});
	}

	// Main Thread
	DoWorkerJob(service);

	GThreadManager->Join();
}