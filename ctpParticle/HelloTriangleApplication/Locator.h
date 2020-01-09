#pragma once

class Timer;
class Devices;

struct Locator
{
public:

	static void InitTimer(Timer* _timer){ timer = _timer; }

	static Timer* GetTimer() { return timer; }

	static void InitDevices(Devices* _devices) { devices = _devices; }

	static Devices* GetDevices() { return devices; }

private:
	static Timer* timer;
	static Devices* devices;
};

