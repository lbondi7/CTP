#pragma once
#include "Object.h"


class Camera : public Object
{

public:
	Camera() = default;
	~Camera() = default;

	void Setup(const glm::vec3& pos, const glm::vec3& lookAt, bool _freeCam = true);

	void Update();

	void Orbit();

	void FreeCam();

	
	void LookAtPos(const glm::vec3& _lookAt) { look_at_pos = _lookAt; }

	const glm::mat4& ViewMatrix() { return viewMatrix; }


private:

	glm::vec3 world_up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 camUp = { 0.0f, 1.0f, 0.0f };
	glm::vec3 cam_forward = { 0.0f, 0.0f, -1.0f };
	glm::vec3 cam_right = { -1.0f, 0.0f, -1.0f };
	glm::vec3 look_at_pos;
	glm::vec3 rotation;
	glm::vec3 rotation_no_y;
	glm::mat4 viewMatrix;

	float zoom_speed = 50.0f;
	float look_at_pos_distance = 500.0f;
	float x_angle = 4.0f;
	float y_angle = 0.0f;
	float orbit_speed = 0.2f;

	float yaw, pitch;

	bool freeCam = true;

};

