#pragma once

struct TimeItem
{
	TimeItem(uint64 tick, weak_ptr<JobQueue> owner, JobRef job)
		: tickTime(tick), owner(owner), job(job) { }

	bool operator<(const TimeItem& other) const { return tickTime > other.tickTime; }
	bool compareTick(const uint64 currentTick) { return tickTime < currentTick; }

	uint64 tickTime;
	weak_ptr<JobQueue> owner;
	JobRef job;
};

class JobTimer
{
	SINGLETON(JobTimer)
public:
	void			Reserve(uint64 afterTick, weak_ptr<JobQueue> owner, JobRef job);
	void			Distribute(uint64 startTick);
	void			Clear();

private:
	USE_LOCK;
	priority_queue<TimeItem, vector<TimeItem>, std::less<TimeItem>> _timers;
};