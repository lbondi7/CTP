#pragma once

class Timer;

struct Locator
{
public:

	static void InitTimer(Timer* _timer){ timer = _timer; }

	static Timer* GetTimer() { return timer; }

private:
	static Timer* timer;
};

