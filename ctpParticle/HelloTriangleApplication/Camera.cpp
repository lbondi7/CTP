#include "Camera.h"

#include "Locator.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Timer.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <iostream>

void Camera::Setup(const glm::vec3& pos, const glm::vec3& lookAt, bool _freeCam)
{
	transform.pos = pos;
	freeCam = _freeCam;

	look_at_pos = lookAt;

	auto diff = glm::vec3(sin(x_angle) * look_at_pos_distance, sin(y_angle) * look_at_pos_distance, cos(x_angle) * look_at_pos_distance)
		- look_at_pos;

	diff = glm::normalize(diff);
	//transform.pos =  diff * distFromLookAtPos;
	viewMatrix = glm::lookAt(transform.pos, lookAt, world_up);
}

void Camera::Update()
{
	if (freeCam)
	{
		FreeCam();
	}
	else
	{
		Orbit();
	}
}

void Camera::Orbit() 
{
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_4))
	{
		x_angle -= orbit_speed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_6))
	{
		x_angle += orbit_speed * Locator::GetTimer()->DeltaTime();
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_8) && y_angle < 1.5f)
	{
		y_angle += orbit_speed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_5) && y_angle > -1.5f)
	{
		y_angle -= orbit_speed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_7))
	{
		look_at_pos_distance += zoom_speed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_9) && look_at_pos_distance > 5.0f)
	{
		look_at_pos_distance -= zoom_speed * Locator::GetTimer()->DeltaTime();
	}

	auto diff = glm::vec3(sin(x_angle) * look_at_pos_distance, sin(y_angle) * look_at_pos_distance, cos(x_angle) * look_at_pos_distance)
		- look_at_pos;

	diff = glm::normalize(diff);

	transform.pos = diff * look_at_pos_distance;

	viewMatrix = glm::lookAt(transform.pos, look_at_pos, world_up);
}

void Camera::FreeCam()
{
	if ((!Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT) &&
		!Locator::GetKeyboard()->IsKeyRepeat(GLFW_KEY_LEFT_SHIFT)) &&
		Locator::GetMouse()->MousePosChanged())
	{
		auto mouseMove = Locator::GetMouse()->Difference();
		yaw += mouseMove.x;
		pitch += mouseMove.y;


		if (pitch > 89.0f)
		{
			pitch = 89.0f;
		}
		else if (pitch < -89.0f)
		{
			pitch = -89.0f;
		}

		rotation_no_y.x = cos(glm::radians(yaw));
		rotation_no_y.z = sin(glm::radians(yaw));
		cam_right = glm::normalize(rotation_no_y);
		rotation.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		rotation.y = sin(glm::radians(pitch));
		rotation.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cam_forward = glm::normalize(rotation);
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_A))
	{
		if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
			Locator::GetKeyboard()->IsKeyRepeat(GLFW_KEY_LEFT_SHIFT))
		{
			//transform.pos -= cam_forward_no_y * Locator::GetTimer()->DeltaTime();
		}
		else
		{
			transform.pos -= glm::normalize(glm::cross(cam_forward, world_up)) * Locator::GetTimer()->DeltaTime() * zoom_speed;
		}
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_D))
	{
		if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
			Locator::GetKeyboard()->IsKeyRepeat(GLFW_KEY_LEFT_SHIFT))
		{
			//transform.pos += cam_forward_no_y * Locator::GetTimer()->DeltaTime();
		}
		else
		{
			transform.pos += glm::normalize(glm::cross(cam_forward, world_up)) * Locator::GetTimer()->DeltaTime() * zoom_speed;
		}
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_W))
	{
		if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_LEFT_SHIFT) ||
			Locator::GetKeyboard()->IsKeyRepeat(GLFW_KEY_LEFT_SHIFT))
		{
			transform.pos += glm::normalize(glm::cross(cam_right, world_up)) * Locator::GetTimer()->DeltaTime() * zoom_speed;
		}
		else
		{
			transform.pos += cam_forward * Locator::GetTimer()->DeltaTime() * zoom_speed;
		}
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_S))
	{
		transform.pos -= cam_forward * Locator::GetTimer()->DeltaTime() * zoom_speed;
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_Q))
	{
		transform.pos -= world_up * Locator::GetTimer()->DeltaTime() * zoom_speed;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_E))
	{
		transform.pos += world_up * Locator::GetTimer()->DeltaTime() * zoom_speed;
	}

	viewMatrix = glm::lookAt(transform.pos, transform.pos + cam_forward, world_up);
}

