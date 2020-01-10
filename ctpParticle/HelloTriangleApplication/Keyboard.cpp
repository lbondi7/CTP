#include "Keyboard.h"

Keyboard::Keyboard(GLFWwindow* _window)
{
	window = _window;
	keys.reserve(256);
}

Keyboard::~Keyboard()
{
	window = nullptr;
	keys.clear();
}

void Keyboard::SetKey(int keyNum, int state)
{
}

bool Keyboard::IsKeyPressed(int keyNum)
{
	return glfwGetKey(window, keyNum) == GLFW_PRESS;
}

bool Keyboard::IsKeyReleased(int keyNum)
{
	return glfwGetKey(window, keyNum) == GLFW_RELEASE;
}

bool Keyboard::IsKeyRepeat(int keyNum)
{
	return glfwGetKey(window, keyNum) == GLFW_REPEAT;
}
