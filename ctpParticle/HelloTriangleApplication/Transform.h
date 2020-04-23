#pragma once

#include "glm/glm.hpp"

struct Transform
{
	glm::vec3 position = {0, 0, 0};
	glm::vec3 rot = { 0, 0, 0 };
	glm::vec3 scale = { 1, 1, 1 };


	bool operator==(const Transform& other) const;

	void operator=(const Transform& other);

};