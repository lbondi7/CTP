#pragma once

#include "Vertex.h"

struct Mesh
{
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	void loadModel(const char* modelPath);
};

