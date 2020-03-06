#include "Mesh.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <stdexcept>
#include <unordered_map>
#include <random>
#include <filesystem>
#include <iostream>
#include <thread>
#include <future>
#include <mutex>

std::mutex mut;

void Mesh::LoadFile(const std::string& mesh)
{
	if (meshes.find(mesh) != meshes.end())
	{
		std::cout << "Mesh is already loaded!" << std::endl;
		return;
	}

	meshes[mesh] = meshCount;
	++meshCount;
	vertices.resize(meshCount);
	indices.resize(meshCount);

	std::string filePath = "models/" + mesh + ".obj";

	LoadMesh(mesh, filePath);
}

void LoadMeshes(std::string filePath,
	std::vector< std::vector<Vertex>>* vertices, std::vector< std::vector<uint32_t>>* indices, int mesh, int* modelsReady)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			vertex.color = { 0.8f, 0.8f, 0.8f, 0.4f };

			//std::lock_guard<std::mutex> lock(mut);
			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>((*vertices)[mesh].size());
				(*vertices)[mesh].push_back(vertex);
			}

			(*indices)[mesh].push_back(uniqueVertices[vertex]);
		}
	}

	std::lock_guard<std::mutex> lock(mut);
	(*modelsReady)++;
}

void Mesh::LoadAll()
{

	std::string dir = "models";

	for (const auto& entry : std::filesystem::directory_iterator(dir))
	{
		std::string mesh = entry.path().filename().string();

		if (mesh.substr(mesh.size() - 3, 3) != "obj")
			continue;

		mesh.resize(mesh.size() - 4);

		meshes[mesh] = meshCount;
		++meshCount;
		vertices.resize(meshCount);
		indices.resize(meshCount);

		std::string filePath = dir + "/" + mesh + ".obj";

		LoadMesh(mesh, filePath);
	}

	//int modelsReady = 0;
	//for (const auto& entry : std::filesystem::directory_iterator(dir))
	//{
	//	std::string mesh = entry.path().filename().string();
	//	mesh.resize(mesh.size() - 4);
	//	std::string filePath = dir + "/" + mesh + ".obj";
	//	std::thread th(LoadMeshes, filePath, &vertices, &indices, meshes[mesh], &modelsReady);
	//	//if (modelCount == (meshCount - 1))
	//	//	th.join();
	//	//else
	//	th.join();
	//}


}

void Mesh::LoadMesh(const std::string& mesh, const std::string& filePath)
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())) {
		throw std::runtime_error(warn + err);
	}

	std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

	for (const auto& shape : shapes) {
		for (const auto& index : shape.mesh.indices) {
			Vertex vertex = {};

			vertex.pos = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};

			vertex.texCoord = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
			};

			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};

			if(!materials.empty())
				vertex.color = { 0.8f * materials[0].specular[0], 0.8f * materials[0].specular[0], 0.8f * materials[0].specular[0], 0.4f };
			else
				vertex.color = { 0.8f, 0.8f, 0.8f, 0.4f };


			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices[meshes[mesh]].size());
				vertices[meshes[mesh]].push_back(vertex);
			}

			indices[meshes[mesh]].push_back(uniqueVertices[vertex]);
		}
	}
	int x = 0;
}
