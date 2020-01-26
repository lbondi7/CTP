#pragma once

class Timer;
class Devices;
class Keyboard;
struct Mesh;
struct Image;

struct Locator
{
public:

	static void InitTimer(Timer* _timer){ timer = _timer; }

	static Timer* GetTimer() { return timer; }

	static void InitKeyboard(Keyboard* _key) { keyboard = _key; }

	static Keyboard* GetKeyboard() { return keyboard; }

	static void InitDevices(Devices* _devices) { devices = _devices; }

	static Devices* GetDevices() { return devices; }

	static void InitMeshes(Mesh* _mesh) { mesh = _mesh; }

	static Mesh* GetMesh() { return mesh; }

	static void InitImages(Image* _image) { image = _image; }

	static Image* GetImage() { return image; }

private:
	static Timer* timer;
	static Devices* devices;
	static Keyboard* keyboard;
	static Mesh* mesh;
	static Image* image;
};

