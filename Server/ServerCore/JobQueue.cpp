#include "pch.h"
#include "JobQueue.h"

void JobQueue::Push(JobRef job, bool isExcutePush)
{
	int32 size = _insideQueue.fetch_add(1);
	_queue.Push(job);

	if (size == 0) {
		if (isExcutePush)
			Excute();
		else
			GQUEUE->Push(shared_from_this());
	}
}

void JobQueue::Excute()
{
	bool expected = false;
	if (_isExcute.compare_exchange_strong(expected, true))
	{
		int64 startTick = GetTickCount64();

		while (true)
		{
			vector<JobRef> jobs;
			if (_queue.PopAll(jobs) == false)
				break;

			for (auto job : jobs)
			{
				job->Excute();
			}

			_insideQueue.fetch_sub(static_cast<int32>(jobs.size()));

			int64 endTick = GetTickCount64();

			if (endTick - startTick > WORK_TICK)
			{
				GQUEUE->Push(shared_from_this());
				break;
			}
		}

		_isExcute.store(false);
	}
}