#include "PoolableThread.h"

#include <iostream>
#include <chrono>
#include "DebugPrinter.h"

using namespace std::chrono_literals;

PoolableThread::PoolableThread()
{
	alive = true;
	thread = std::thread(&PoolableThread::Run, this);
	thread.detach();
}

PoolableThread::PoolableThread(int _id)
{
	alive = true;
	id = _id;
	thread = std::thread(&PoolableThread::Run, this);
}

PoolableThread::~PoolableThread()
{

}

void PoolableThread::Run()
{
	while (alive)
	{
		if (!idle && func)
		{
			func();
			func = nullptr;
			idle = true;
		}
		else
			std::this_thread::sleep_for(0.5ms);
	}
	destroyed = true;
}
