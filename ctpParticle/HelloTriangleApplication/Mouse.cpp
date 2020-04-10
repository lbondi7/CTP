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
	if (!first_update)
	{
		last_mouse_pos = mouse_pos;
	}

	first_update = false;

	last_mouse_pos = mouse_pos;
	double x_pos, y_pos;
	glfwGetCursorPos(window, &x_pos, &y_pos);

	mouse_pos.x = x_pos;
	mouse_pos.y = y_pos;
}

glm::vec2 Mouse::Difference()
{
	return glm::vec2(mouse_pos.x - last_mouse_pos.x, last_mouse_pos.y - mouse_pos.y) * sensitivity;
}

bool Mouse::MousePosChanged()
{
	return (last_mouse_pos.x != mouse_pos.x) || (last_mouse_pos.y != mouse_pos.y);
}