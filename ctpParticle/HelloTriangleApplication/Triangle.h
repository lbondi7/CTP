#pragma once

#include "glm/glm.hpp"
#include <array>

struct Triangle
{

	Triangle() = default;
	Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 _pos);

	float dot2(glm::vec3 v);

	float udTriangle(glm::vec3 p);

	glm::vec3 pos;
	glm::vec3 center;
	glm::vec3 normal;
	std::array<glm::vec3, 3> vertices;
	std::array<glm::vec3, 3> edges;

};

