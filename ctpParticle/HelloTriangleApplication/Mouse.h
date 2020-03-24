#pragma once

#include "glm/glm.hpp"
#include "GLFW/glfw3.h"

#include <memory>

class Mouse
{
public:
	Mouse(GLFWwindow* _window);
	~Mouse();

	void Update();

	glm::vec2 Difference();

	bool MousePosChanged();


private:

	GLFWwindow* window = nullptr;

	glm::vec2 mousePos;
	glm::vec2 lastMousePos = {400, 300};
	float sensitivity = 0.5f;
	bool firstUpdate = true;

};

