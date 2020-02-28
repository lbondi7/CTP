#pragma once

#include "Vertex.h"
#include <map>
#include <string>

struct Mesh
{
	std::vector<std::vector<Vertex>> vertices;
	std::vector<std::vector<uint32_t>> indices;

	std::map<std::string, int> meshes;

	int meshCount = 0;


	void LoadFile(const std::string& mesh);

	void LoadAll();

private:

	void LoadMesh(const std::string& mesh, const std::string& filePath);
};

