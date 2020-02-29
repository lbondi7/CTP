#include "Timer.h"

Timer::Timer()
{
	newDelta = std::chrono::high_resolution_clock::now();
}

float Timer::FixedDeltaTime()
{
	float delta = std::chrono::duration<float_t, std::ratio<1, 60>>().count();
	return delta;
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
	oldTime = time;
}
