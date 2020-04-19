#pragma once

#include "Transform.h"

#include "glm/glm.hpp"
#include <array>

struct Triangle
{
	Triangle() = default;
	Triangle(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, const Transform& transform);

	void Update(const glm::vec3& p);

	float ShorestDistance(glm::vec3 p);

	float UdTriangle(glm::vec3 p);

	void operator =(const Triangle& other);


	glm::vec4 center;
	glm::vec4 normal;
	std::array<glm::vec4, 3> localVertices;
	std::array<glm::vec4, 3> vertices;
	std::array<glm::vec4, 3> edges;
	glm::vec4 other_edge;
	glm::vec4 min;
	glm::vec4 max;
};

