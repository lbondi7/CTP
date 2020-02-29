#pragma once

#include <chrono>

class Timer
{
public:
	Timer();
	~Timer() = default;

	float FixedDeltaTime();
	float DeltaTime();
	void GetTimePoint();
	void GetTimePoint(float time);

private:

	std::chrono::high_resolution_clock::time_point newDelta;
	std::chrono::duration<double, std::chrono::seconds::period> deltaTime;

	float dt = 0.0f;
	float oldTime = 0.0f;

};

