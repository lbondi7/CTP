#include "ThreadManager.h"
#include "DebugPrinter.h"

#include <iostream>
using namespace std::chrono_literals;

std::mutex ThreadManager::threads_mutex;

const int max_thread_count = std::thread::hardware_concurrency() * 2;

ThreadManager::ThreadManager(int num_of_threads, bool override_max_threads)
{
	Init(num_of_threads, override_max_threads);
}

ThreadManager::~ThreadManager()
{
	while (!threads.empty())
	{
		int nullCount = 0;
		for (int i = 0; i < threads.size(); ++i)
		{
			if (threads[i])
			{
				if (threads[i]->IsAlive())
					threads[i]->SetAlive(false);

				if (threads[i]->IsDestroyed())
				{
					threads[i].release();
					//std::cout << "thread: " << i << " was destroyed!" << std::endl;
				}
			}
			else
				nullCount++;
		}
		if (nullCount == threads.size())
			threads.clear();
	}
}

void ThreadManager::Init(int num_of_threads, bool override_max_threads)
{
	if (!override_max_threads)
	{
		if (num_of_threads > max_thread_count)
			num_of_threads = max_thread_count;
	}

	for (size_t i = 0; i < num_of_threads; i++)
	{
		threads.emplace_back(std::move(std::make_unique<PoolableThread>(i)));
	}
	

}

std::function<void()>& ThreadManager::SetTask(bool _perminate)
{
	while (true)
	{
		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i])
			{
				if (threads[i]->IsAlive() && threads[i]->IsIdle() && !threads[i]->IsPermanent())
				{
					threads[i]->SetIdle(false);
					threads[i]->SetPermanent(_perminate);
					return threads[i]->func;
				}
			}
		}
	}
}

bool ThreadManager::IsExecutingTask()
{
	for (size_t i = 0; i < threads.size(); i++)
	{
		if (threads[i])
		{
			if (threads[i]->IsAlive() && !threads[i]->IsIdle())
			{
				return true;
			}
		}
	}
	return false;
}

void ThreadManager::WaitForThreads()
{
	std::lock_guard<std::mutex> lock(threads_mutex);
	bool wait = true;
	while (wait)
	{
		wait = false;
		for (size_t i = 0; i < threads.size(); i++)
		{
			if (threads[i] && threads[i]->IsAlive() &&
				!threads[i]->IsPermanent() && !threads[i]->IsIdle())
			{
				wait = true;
			}
		}
		//std::this_thread::sleep_for(0.05ms);
	}
}
