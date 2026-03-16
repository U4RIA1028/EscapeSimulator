#include "pch.h"
#include "Lock.h"
#include "JobTimer.h"
#include "JobQueue.h"

void JobTimer::Reserve(uint64 afterTick, weak_ptr<JobQueue> owner, JobRef job)
{
	WRITE_LOCK;
	uint64 after = GetTickCount64() + afterTick;
	_timers.push(TimeItem{ after, owner, job });
}

void JobTimer::Distribute(uint64 startTick)
{
	WRITE_LOCK;

	while (_timers.size() != 0)
	{
		TimeItem item = _timers.top();

		if (item.compareTick(startTick))
		{
			if (JobQueueRef jobRef = item.owner.lock())
				jobRef->Push(item.job);
			else
				cout << item.owner.expired() << endl;
			_timers.pop();
		}
		else
			break;

		uint64 endTick = GetTickCount64();
		if (endTick - startTick > WORK_TICK)
			break;
	}
}

void JobTimer::Clear()
{

}
