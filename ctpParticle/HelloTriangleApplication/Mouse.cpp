#include "Mouse.h"

#include "Locator.h"
#include "Keyboard.h"

Mouse::Mouse(GLFWwindow* _window) : window(_window)
{
	
}

Mouse::~Mouse()
{
}

void Mouse::Update()
{
	if (!firstUpdate)
	{
		lastMousePos = mousePos;
	}

	firstUpdate = false;

	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	mousePos.x = xPos;
	mousePos.y = yPos;
}

glm::vec2 Mouse::Difference()
{
	return glm::vec2(mousePos.x - lastMousePos.x, lastMousePos.y - mousePos.y) * sensitivity;
}

bool Mouse::MousePosChanged()
{
	return (lastMousePos.x != mousePos.x) || (lastMousePos.y != mousePos.y);
}