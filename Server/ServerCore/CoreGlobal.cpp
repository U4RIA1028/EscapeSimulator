#include "pch.h"
#include "CoreGlobal.h"
#include "ThreadManager.h"
#include "DeadLockProfiler.h"

std::unique_ptr<ThreadManager> GThreadManager = make_unique<ThreadManager>();
std::unique_ptr<DeadLockProfiler> GDeadLockProfiller = make_unique<DeadLockProfiler>();
