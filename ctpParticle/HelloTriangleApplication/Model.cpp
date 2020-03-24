#include "Model.h"
#include "Devices.h"
#include "Mesh.h"
#include "Locator.h"

#include <stdexcept>
#include <unordered_map>
#include <random>

Model::~Model()
{
}

void Model::Load(const std::string& filepath, VkQueue queue)
{
	Destroy();

	vertices = Locator::GetMesh()->vertices[Locator::GetMesh()->meshes[filepath]];
	indices = Locator::GetMesh()->indices[Locator::GetMesh()->meshes[filepath]];

	CreateBuffers(queue);
}

void Model::Update()
{

	vertex.CopyMem(vertices.data(), vertex.size);

}

void Model::CreateBuffers(VkQueue queue)
{
	vertex.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, vertices.size() * sizeof(vertices[0]));

	vertex.StageBuffer(vertex.size, queue, vertices.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	index.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, indices.size() * sizeof(indices[0]));

	index.StageBuffer(index.size, queue, indices.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

void Model::Destroy()
{
	vertex.DestoryBuffer();
	index.DestoryBuffer();
	uniform.DestoryBuffer();

	vertices.clear();
	indices.clear();
}