#pragma once
#include "PoolableThread.h"

#include <vector>
#include <functional>
#include <optional>
#include <mutex>

class ThreadManager
{
public:

	ThreadManager() = default;
	ThreadManager(int num_of_threads, bool override_max_threads = false);
	~ThreadManager();

	void Init(int num_of_threads, bool override_max_threads = false);

	std::function<void()>& SetTask(bool _perminate = false);

	const int ThreadCount() { return static_cast<int>(threads.size()); }

	bool IsExecutingTask();

	void WaitForThreads();


private:

	std::vector<std::unique_ptr<PoolableThread>> threads;

	static std::mutex threads_mutex;
};