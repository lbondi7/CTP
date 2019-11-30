#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer() = default;

	 float DeltaTime();
	 void GetTimePoint();
private:

	std::chrono::high_resolution_clock::time_point oldDelta;
	std::chrono::duration<float, std::chrono::milliseconds::period> deltaTime;
};

