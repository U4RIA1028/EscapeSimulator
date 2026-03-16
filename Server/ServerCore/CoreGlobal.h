#pragma once

class ThreadManager;
class DeadLockProfiler;
extern std::unique_ptr<ThreadManager> GThreadManager;
extern std::unique_ptr<DeadLockProfiler> GDeadLockProfiller;