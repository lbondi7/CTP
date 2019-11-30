#include "Timer.h"

Timer::Timer()
{
	oldDelta = std::chrono::high_resolution_clock::now();
}

float Timer::DeltaTime()
{
	return deltaTime.count() / 1000.0f;
}

void Timer::GetTimePoint()
{
	auto currentDelta = std::chrono::high_resolution_clock::now();
	deltaTime = std::chrono::duration<float, std::chrono::milliseconds::period>(currentDelta- oldDelta);
	oldDelta = std::chrono::high_resolution_clock::now();
}
