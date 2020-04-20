#pragma once

#include "Vertex.h"
#include "Triangle.h"

struct Bounds
{
	glm::vec3 min = { INFINITY, INFINITY, INFINITY };
	glm::vec3 max = { -INFINITY, -INFINITY, -INFINITY };

	glm::vec3 center = { -INFINITY, -INFINITY, -INFINITY };
	//size_t number = 0;
	bool IsInside(const glm::vec3& point);

	void Load(const std::vector<Vertex>& vertices);
	void Load(const std::vector<Triangle>& triangles);
	void Load(const std::vector<Triangle*>& triangles);
};

