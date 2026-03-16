#include "pch.h"
#include "GlobalQueue.h"

void GlobalQueue::Push(JobQueueRef ref)
{
	_jobQueue.Push(ref);
}

JobQueueRef GlobalQueue::Pop()
{
	return _jobQueue.Pop();
}
