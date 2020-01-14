#pragma once

#include "Vertex.h"

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	void loadModel(const char* modelPath);

	void changeVertices();

	void SetupBuffers();

};

