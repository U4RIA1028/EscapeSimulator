#pragma once
#include "Session.h"

class SessionManager
{
	SINGLETON(SessionManager)

public:
	void PushSession(SessionRef ref)
	{
		WRITE_LOCK;
		auto tmp = find(_sessions.begin(), _sessions.end(), ref);
		if (tmp != _sessions.end())
			CRASH("Double Push");

		_sessions.push_back(ref);
	}

	void PopSession(SessionRef ref)
	{
		WRITE_LOCK;
		if(_sessions.size() == 0)
			CRASH("Not Have Ref");

		auto tmp = find(_sessions.begin(), _sessions.end(), ref);
		if (tmp == _sessions.end())
			CRASH("Not Have Ref");

		_sessions.erase(tmp);
	}

	void BroadCase(SendBufferRef send)
	{
		WRITE_LOCK;
		for (SessionRef ref : _sessions)
		{
			ref->Send(send);
		}
	}

	int32 GetCurrentSessionCount() { return static_cast<int32>(_sessions.size()); }

private:
	USE_LOCK;

	vector<SessionRef> _sessions;
};