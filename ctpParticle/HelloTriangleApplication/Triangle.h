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


	glm::vec3 center;
	glm::vec3 normal;
	std::array<glm::vec3, 3> localVertices;
	std::array<glm::vec3, 3> vertices;
	std::array<glm::vec3, 3> edges;
	glm::vec3 other_edge;
	glm::vec3 min;
	glm::vec3 max;
};

