#include "Camera.h"

#include "Locator.h"
#include "Keyboard.h"
#include "Timer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

void Camera::Setup(const glm::vec3& pos, const glm::vec3& lookAt)
{
	transform.pos = pos;
	lookAtPos = lookAt;

	auto diff = glm::vec3(sin(angleX) * distFromOrigin, sin(angleY) * distFromOrigin, cos(angleX) * distFromOrigin)
		- lookAtPos;

	diff = glm::normalize(diff);
	transform.pos =  diff * distFromOrigin;
	viewMatrix = glm::lookAt(transform.pos, lookAtPos, up);
}

void Camera::Update()
{
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_4))
	{
		angleX -= angleSpeed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_6))
	{
		angleX += angleSpeed * Locator::GetTimer()->DeltaTime();
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_8) && angleY < 1.5f)
	{
		angleY += angleSpeed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_5) && angleY > -1.5f)
	{
		angleY -= angleSpeed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_7))
	{
		distFromOrigin += zoomSpeed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_9) && distFromOrigin > 5.0f)
	{
		distFromOrigin -= zoomSpeed * Locator::GetTimer()->DeltaTime();
	}

	auto diff = glm::vec3(sin(angleX) * distFromOrigin, sin(angleY) * distFromOrigin, cos(angleX) * distFromOrigin)
		- lookAtPos;

	diff = glm::normalize(diff);

	transform.pos = diff * distFromOrigin;

	viewMatrix = glm::lookAt(transform.pos, lookAtPos, up);
}
