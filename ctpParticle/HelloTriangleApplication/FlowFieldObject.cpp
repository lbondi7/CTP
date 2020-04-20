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
		//Triangle tri(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos);

		//triangles.push_back(tri);
	}
}

void FfObject::Load(const std::string& filepath, const Transform& _transform)
{
	Destroy();

	vertices = Locator::GetMesh()->vertices[Locator::GetMesh()->meshes[filepath]];
	indices = Locator::GetMesh()->indices[Locator::GetMesh()->meshes[filepath]];

	transform = _transform;

	for (size_t i = 0; i < indices.size(); i += 3)
	{
		Triangle tri(vertices[indices[i]].pos, vertices[indices[i + 1]].pos, vertices[indices[i + 2]].pos, _transform);

		triangles.emplace_back(tri);
	}

	if (triangles.size() > 50)
	{
		for (auto tri : triangles)
		{
			brokenTriangles[GetGridNum(tri.center)].emplace_back(tri);
			//if (tri.center.x < transform.pos.x)
			//{
			//	if (tri.center.z < transform.pos.z)
			//	{
			//		if (tri.center.y < transform.pos.y)
			//		{
			//			brokentriangles[0].emplace_back(tri);
			//		}
			//		else
			//		{
			//			brokentriangles[4].emplace_back(tri);
			//		}
			//	}
			//	else
			//	{
			//		if (tri.center.y < transform.pos.y)
			//		{
			//			brokentriangles[1].emplace_back(tri);
			//		}
			//		else
			//		{
			//			brokentriangles[5].emplace_back(tri);
			//		}
			//	}
			//}
			//else
			//{
			//	if (tri.center.z < transform.pos.z)
			//	{
			//		if (tri.center.y < transform.pos.y)
			//		{
			//			brokentriangles[3].emplace_back(tri);
			//		}
			//		else
			//		{
			//			brokentriangles[7].emplace_back(tri);
			//		}
			//	}
			//	else
			//	{
			//		if (tri.center.y < transform.pos.y)
			//		{
			//			brokentriangles[2].emplace_back(tri);
			//		}
			//		else
			//		{
			//			brokentriangles[6].emplace_back(tri);
			//		}
			//	}
			//}
		}
	}

	for (auto& tri: triangles)
	{
		min.x = std::min(min.x, tri.min.x);
		min.y = std::min(min.y, tri.min.y);
		min.z = std::min(min.z, tri.min.z);

		max.x = std::max(max.x, tri.max.x);
		max.y = std::max(max.y, tri.max.y);
		max.z = std::max(max.z, tri.max.z);
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

int FfObject::GetGridNum(const glm::vec3& p1)
{
	if (p1.x < transform.position.x)
	{
		if (p1.z < transform.position.z)
		{
			if (p1.y < transform.position.y)
			{
				return 0;
			}
			else
			{
				return 4;
			}
		}
		else
		{
			if (p1.y < transform.position.y)
			{
				return 1;
			}
			else
			{
				return 5;
			}
		}
	}
	else
	{
		if (p1.z < transform.position.z)
		{
			if (p1.y < transform.position.y)
			{
				return 3;
			}
			else
			{
				return 7;
			}
		}
		else
		{
			if (p1.y < transform.position.y)
			{
				return 2;
			}
			else
			{
				return 6;
			}
		}
	}
}
