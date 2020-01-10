#pragma once

class Timer;
class Devices;
class Keyboard;

struct Locator
{
public:

	static void InitTimer(Timer* _timer){ timer = _timer; }

	static Timer* GetTimer() { return timer; }

	static void InitKeyboard(Keyboard* _key) { keyboard = _key; }

	static Keyboard* GetKeyboard() { return keyboard; }

	static void InitDevices(Devices* _devices) { devices = _devices; }

	static Devices* GetDevices() { return devices; }

private:
	static Timer* timer;
	static Devices* devices;
	static Keyboard* keyboard;
};

