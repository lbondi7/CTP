#pragma once

#include "Vertex.h"
#include "Buffer.h"
#include "Transform.h"

struct Model
{

	~Model();

	void Load(const char* modelPath, VkQueue queue);

	void CreateBuffers(VkQueue queue);

	VkDevice device;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	Buffer vertex;
	Buffer index;
	std::vector<Buffer> uniform;

	Transform transform;

	void Load(const char* modelPath);

	void CreateBuffers();

};

