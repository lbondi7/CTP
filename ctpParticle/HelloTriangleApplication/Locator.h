#pragma once

class Timer;
class Devices;
class Keyboard;
class Mouse;
struct Mesh;
struct Image;
struct Shaders;

struct Locator
{
public:

	static void InitTimer(Timer* _timer){ timer = _timer; }

	static Timer* GetTimer() { return timer; }

	static void InitKeyboard(Keyboard* _key) { keyboard = _key; }

	static Keyboard* GetKeyboard() { return keyboard; }

	static void InitMouse(Mouse* _mouse) { mouse = _mouse; }

	static Mouse* GetMouse() { return mouse; }

	static void InitDevices(Devices* _devices) { devices = _devices; }

	static Devices* GetDevices() { return devices; }

	static void InitMeshes(Mesh* _mesh) { mesh = _mesh; }

	static Mesh* GetMesh() { return mesh; }

	static void InitImages(Image* _image) { image = _image; }

	static Image* GetImage() { return image; }

	static void InitShader(Shaders* _shader) { shader = _shader; }

	static Shaders* GetShader() { return shader; }

private:
	static Timer* timer;
	static Devices* devices;
	static Keyboard* keyboard;
	static Mouse* mouse;
	static Mesh* mesh;
	static Image* image;
	static Shaders* shader;
};

