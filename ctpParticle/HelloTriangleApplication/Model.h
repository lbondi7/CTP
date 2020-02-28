#pragma once

#include "Vertex.h"
#include "Buffer.h"
#include "Transform.h"
#include "Triangle.h"

struct Model
{
	virtual ~Model();

	void Load(const std::string& filepath, VkQueue queue);

	void CreateBuffers(VkQueue queue);

	void Destroy();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vertex;
	Buffer index;
	Buffer uniform;
};

struct FfModel : TransformObject
{
	~FfModel();

	void Load(const std::string& filepath);
	void Load(const std::string& filepath, glm::vec3 pos);
	void Destroy();

	void Update();

	std::vector<Triangle> triangles;

private:
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
};

