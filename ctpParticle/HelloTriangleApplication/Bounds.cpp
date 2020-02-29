#include "Bounds.h"

#include <algorithm>

bool Bounds::IsInside(const glm::vec3& point)
{
	return (point.x <= max.x && point.x >= min.x) && 
		(point.y <= max.y && point.y >= min.y) && 
		(point.z <= max.z && point.z >= min.z);
}

void Bounds::Load(const std::vector<Vertex>& vertices)
{
	for (size_t i = 0; i < vertices.size(); i++)
	{
		max.x = std::max(vertices[i].pos.x, max.x);
		max.y = std::max(vertices[i].pos.y, max.y);
		max.z = std::max(vertices[i].pos.z, max.z);

		min.x = std::min(vertices[i].pos.x, min.x);
		min.y = std::min(vertices[i].pos.y, min.y);
		min.z = std::min(vertices[i].pos.z, min.z);
	}

}

void Bounds::Load(const std::vector<Triangle>& triangles)
{
	for (size_t i = 0; i < triangles.size(); ++i)
	{
		for (size_t j = 0; j < 3; ++j)
		{
			max.x = std::max(triangles[i].vertices[j].x, max.x);
			max.y = std::max(triangles[i].vertices[j].y, max.y);
			max.z = std::max(triangles[i].vertices[j].z, max.z);

			min.x = std::min(triangles[i].vertices[j].x, min.x);
			min.y = std::min(triangles[i].vertices[j].y, min.y);
			min.z = std::min(triangles[i].vertices[j].z, min.z);
		}
	}

}
