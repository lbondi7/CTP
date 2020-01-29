#pragma once

#include "Vertex.h"
#include "Buffer.h"
#include "Transform.h"

struct Model
{
	~Model();

	void Load(const std::string& filepath, VkQueue queue);

	void CreateBuffers(VkQueue queue);

	VkDevice device;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vertex;
	Buffer index;
	//std::vector<Buffer> uniform;
	Buffer uniform;

	Transform transform;
};

