#include "pch.h"
#include "IocpCore.h"
#include "IocpEvent.h"
IocpCore::IocpCore()
{
	_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, NULL);
	if (_iocpHandle == INVALID_HANDLE_VALUE)
		CRASH("INVALID_HANDLE_VALUE");
}

IocpCore::~IocpCore()
{
	::CloseHandle(_iocpHandle);
}

bool IocpCore::Register(IocpObjectRef iocpObject)
{
	return ::CreateIoCompletionPort(iocpObject->GetHandle(), _iocpHandle, /*Key*/0, 0);
}

bool IocpCore::Dispatch(uint32 timeoutMs)
{
	DWORD numOfBytes;
	ULONG_PTR key;
	IocpEvent* iocpEvent = nullptr;
	if (GetQueuedCompletionStatus(_iocpHandle, &numOfBytes, &key, reinterpret_cast<LPOVERLAPPED*>(&iocpEvent), timeoutMs))
	{
		IocpObjectRef ref = iocpEvent->_owner;
		ref->Dispatch(iocpEvent, numOfBytes);
	}
	else
	{
		int32 errCode = ::GetLastError();
		if (errCode == WAIT_TIMEOUT)
			return false;
		else
		{
			IocpObjectRef iocpObject = iocpEvent->_owner;
			iocpObject->Dispatch(iocpEvent, numOfBytes);
		}
	}

	return false;
}
