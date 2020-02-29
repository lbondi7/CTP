#include "FlowFieldObject.h"

#include "Locator.h"
#include "Mesh.h"

FfObject::~FfObject()
{
}

void FfObject::Load(const std::string& filepath)
{
	Destroy();

	vertices = Locator::GetMesh()->vertices[Locator::GetMesh()->meshes[filepath]];
	indices = Locator::GetMesh()->indices[Locator::GetMesh()->meshes[filepath]];

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Triangle tri(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos);

		triangles.push_back(tri);
	}
}

void FfObject::Load(const std::string& filepath, glm::vec3 pos)
{
	Destroy();

	vertices = Locator::GetMesh()->vertices[Locator::GetMesh()->meshes[filepath]];
	indices = Locator::GetMesh()->indices[Locator::GetMesh()->meshes[filepath]];

	transform.pos = pos;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Triangle tri(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos, pos);

		triangles.push_back(tri);
	}
}

void FfObject::Destroy()
{
	vertices.clear();
	indices.clear();
	triangles.clear();
}

void FfObject::Update()
{

	//for (size_t i = 0; i < triangles.size(); ++i)
	//{
	//	triangles[i].Update(transform.pos);
	//}
}
