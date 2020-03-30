#include "Timer.h"

Timer::Timer()
{
	newDelta = std::chrono::high_resolution_clock::now();
}

float Timer::FixedDeltaTime()
{
	return 1.0f / 60.0f;
}

float Timer::DeltaTime()
{
	//return deltaTime.count() / 1000.0f;
	return dt;
}

void Timer::GetTimePoint()
{
	auto oldDelta = newDelta;
	newDelta = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<double, std::chrono::seconds::period>(newDelta - oldDelta);
}

void Timer::GetTimePoint(float time)
{
	dt = time - oldTime;
	if (dt > 0.1f)
	{
		dt = 0.1f;
	}
	oldTime = time;
}
