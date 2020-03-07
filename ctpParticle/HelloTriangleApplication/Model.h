#pragma once

#include "Vertex.h"
#include "Buffer.h"
#include "Transform.h"


struct Model
{
	virtual ~Model();

	void Load(const std::string& filepath, VkQueue queue);

	void Update();

	void CreateBuffers(VkQueue queue);

	void Destroy();

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vertex;
	Buffer index;
	Buffer uniform;
};

