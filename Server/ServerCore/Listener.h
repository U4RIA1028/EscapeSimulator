#pragma once
#include <functional>
#include <memory>
#include "Session.h"

class Listener : public IocpObject
{
public:
	Listener(NetAddress addr, int32 max);
	~Listener();

public:
	bool StartAccept();

private:
	void RegisterAccept(AcceptEvent* event);
	void ProcessAccept(AcceptEvent* event);

private:
	/* Handle 및 Accept 실행 */
	virtual HANDLE		GetHandle() override;
	virtual void		Dispatch(class IocpEvent* iocpEvent, int32 numOfBytes = 0) override;

public:
	void SetSessionFunction(std::function<SessionRef()> function) { _sessioFunction = function; }
	void SetIocpCore(IocpCoreRef ref) { _iocpCore = ref; };
	IocpCoreRef GetIocpCore() { return _iocpCore; }

private:
	SOCKET _listenSocket;
	NetAddress _addr;

	int32 _socketCountMax;
	vector<AcceptEvent*> _acceptEvents;

	IocpCoreRef _iocpCore;
	std::function<SessionRef()> _sessioFunction;
};

