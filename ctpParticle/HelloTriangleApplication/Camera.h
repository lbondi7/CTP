#pragma once
#include "Transform.h"


class Camera : public TransformObject
{

public:
	Camera() = default;
	~Camera() = default;

	void Setup(const glm::vec3& pos, const glm::vec3& lookAt);

	void Update();

	
	void LookAtPos(const glm::vec3& _lookAt) { lookAtPos = _lookAt; }

	const glm::mat4& ViewMatrix() { return viewMatrix; }


private:

	glm::vec3 up = { 0.0f, 1.0f, 0.0f };
	glm::vec3 lookAtPos;
	glm::mat4 viewMatrix;

	float zoomSpeed = 100.0f;
	float distFromOrigin = 10.0f;
	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleSpeed = 2.5f;

	glm::vec3 prevUp = { 0.0f, 1.0f, 0.0f };
	glm::vec3 prevLookAtPos;

};

