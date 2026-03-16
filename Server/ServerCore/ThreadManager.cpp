#include "pch.h"
#include "ThreadManager.h"
#include "JobQueue.h"
#include "JobTimer.h"

/*------------------
	ThreadManager
-------------------*/

ThreadManager::ThreadManager()
{
	// Main Thread
	InitTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(function<void(void)> callback)
{
	LockGuard guard(_lock);

	_threads.push_back(thread([=]()
		{
			InitTLS();
			callback();
			DestroyTLS();
		}));
}

void ThreadManager::Join()
{
	for (thread& t : _threads)
	{
		if (t.joinable())
			t.join();
	}
	_threads.clear();
}

void ThreadManager::DoGlobalQueue()
{
	int64 startTick = GetTickCount64();

	while (true)
	{
		JobQueueRef jobQueue = GQUEUE->Pop();
		if (jobQueue == nullptr)
			break;

		jobQueue->Excute();

		int64 endTick = GetTickCount64();
		if (endTick - startTick > WORK_TICK)
			break;
	}
}

void ThreadManager::DoGlobalTimer()
{
	GTIMER->Distribute(GetTickCount64());
}

void ThreadManager::InitTLS()
{
	static Atomic<uint32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyTLS()
{

}