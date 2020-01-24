#include "Model.h"
#include "Devices.h"
#include "Locator.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <stdexcept>
#include <unordered_map>
#include <random>

Model::~Model()
{
	vertex.DestoryBuffer();
	index.DestoryBuffer();
	for (size_t i = 0; i < uniform.size(); ++i)
	{
		uniform[i].DestoryBuffer();
	}
	uniform.clear();
}

void Model::Load(const char* modelPath, VkQueue queue)
{
	device = Locator::GetDevices()->GetDevice();
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warn, err;

	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath)) {
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

			//std::random_device rd;
			//std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
			//vertex.color = { uniformDist(rd) ,uniformDist(rd) , uniformDist(rd) , 1.0f };
			vertex.color = { 1.0f, 1.0f, 1.0f, 1.0f };

			if (uniqueVertices.count(vertex) == 0) {
				uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
				vertices.push_back(vertex);
			}

			indices.push_back(uniqueVertices[vertex]);
		}
	}

	CreateBuffers(queue);
}

void Model::CreateBuffers(VkQueue queue)
{
	vertex.CreateBuffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.size() * sizeof(vertices[0]));

	vertex.StageBuffer(vertex.size, queue, vertices.data());

	index.CreateBuffer(device, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.size() * sizeof(indices[0]));

	index.StageBuffer(index.size, queue, indices.data());
}
