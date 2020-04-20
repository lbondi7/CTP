#pragma once

#include "Object.h"
#include "Vertex.h"
#include "Triangle.h"

#include <vector>

struct FfObject : public Object
{
	~FfObject();

	void Load(const std::string& filepath);
	void Load(const std::string& filepath, const Transform& _transform);
	void Destroy();

	void Update();

	int GetGridNum(const glm::vec3& p1);

	std::vector<Triangle> triangles;

	std::array<std::vector<Triangle>, 8> brokenTriangles;

	glm::vec4 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX };
	glm::vec4 min = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};
