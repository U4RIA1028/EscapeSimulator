#pragma once
#include "LockQueue.h"
#include "JobTimer.h"

using CallBack = std::function<void()>;

class Job
{
public:
	Job() { }
	Job(CallBack&& func) : _func(std::move(func)) { }

	template<typename T, typename Ret, typename... Args>
	Job(shared_ptr<T> obj, Ret(T::* memfunc)(Args...), Args... args)
	{
		_func = [obj, memfunc, args...]() {
			(obj.get()->*memfunc)(args...);
		};
	}

	void Excute() { _func(); }

private:
	CallBack _func;
};

class JobQueue : public enable_shared_from_this<JobQueue>
{
public:
	void DoAsync(CallBack&& func)
	{
		JobRef job = make_shared<Job>(std::move(func));
		Push(job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> ptr = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<Job>(ptr, memFunc, std::forward<Args>(args)...);
		Push(job);
	}

	void DoTimer(uint64 time, CallBack&& func)
	{
		JobRef job = make_shared<Job>(std::move(func));
		GTIMER->Reserve(time, shared_from_this(), job);
	}

	template<typename T, typename Ret, typename... Args>
	void DoTimer(uint64 time, Ret(T::* memFunc)(Args...), Args... args)
	{
		shared_ptr<T> owner = static_pointer_cast<T>(shared_from_this());
		JobRef job = make_shared<Job>(owner, memFunc, std::forward<Args>(args)...);
		GTIMER->Reserve(time, shared_from_this(), job);
	}

public:
	void Excute();
	void Push(JobRef job, bool isExcutePush = true);

private:
	USE_LOCK;
	LockQueue<JobRef> _queue;
	atomic<int32> _insideQueue = 0;
	atomic<bool> _isExcute = false;
};

