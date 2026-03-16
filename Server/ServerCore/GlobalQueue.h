#pragma once

class GlobalQueue
{
	SINGLETON(GlobalQueue)

public:
	void		Push(JobQueueRef ref);
	JobQueueRef Pop();

private:
	LockQueue<JobQueueRef>			_jobQueue;
	priority_queue<JobTimeRef>		_jobTimer;
};

