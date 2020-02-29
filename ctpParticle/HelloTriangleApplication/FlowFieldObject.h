#pragma once

#include "Object.h"
#include "Vertex.h"
#include "Triangle.h"

#include <vector>

struct FfObject : public Object
{
	~FfObject();

	void Load(const std::string& filepath);
	void Load(const std::string& filepath, glm::vec3 pos);
	void Destroy();

	void Update();

	std::vector<Triangle> triangles;

private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};
