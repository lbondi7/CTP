#pragma once

#include "GLFW/glfw3.h"
#include <vector>

enum class State : int
{
	NO_INPUT = 0,
	PRESSED = 1,
	RELEASED = 2,
	REPEATED = 3
};

struct Key
{
	int keyNum = 0;
	State state = State::NO_INPUT;

};

class Keyboard
{
public:

	Keyboard(GLFWwindow* _window);
	~Keyboard();

	void SetKey(int keyNum, int state);
	bool IsKeyPressed(int keyNum);

	bool IsKeyReleased(int keyNum);

	bool IsKeyRepeat(int keyNum);

	GLFWwindow* window = nullptr;

private:

	std::vector<Key> keys;


};

