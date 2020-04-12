#include "LGH.h"

#include <algorithm>

LGH::~LGH()
{
}

void LGH::Create(const std::vector<Light>& pl)
{
	cube_per_axis_squared = (int)std::pow(cube_per_axis, 2);
	cube_per_axis_cubed = (int)std::pow(cube_per_axis, 3);

	Recreate(pl);

	//for (auto point : pl)
	//{
	//	bound.min.x = std::min(point.pos.x, bound.min.x);
	//	bound.min.y = std::min(point.pos.y, bound.min.y);
	//	bound.min.z = std::min(point.pos.z, bound.min.z);

	//	bound.max.x = std::max(point.pos.x, bound.max.x);
	//	bound.max.y = std::max(point.pos.y, bound.max.y);
	//	bound.max.z = std::max(point.pos.z, bound.max.z);
	//}

	//glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / (float)cube_per_axis;

	//lights.resize(cube_per_axis_cubed);
	//bounds.resize(cube_per_axis_cubed);

	//float xVal = bound.min.x;
	//float yVal = bound.min.y;
	//float zVal = bound.min.z;

	//int mod4;
	//int mod16;

	//for (size_t i = 0; i < bounds.size(); ++i)
	//{
	//	bounds[i].min = glm::vec3(xVal, yVal, zVal);
	//	bounds[i].max = bounds[i].min + quadSize;

	//	bounds[i].center = bounds[i].min + (quadSize / 2.0f);

	//	xVal += quadSize.x;

	//	mod4 = i % cube_per_axis;
	//	if (mod4 == 0 && i != 0)
	//	{
	//		xVal = bound.min.x;
	//		yVal += quadSize.y;
	//		mod16 = i % cube_per_axis_squared;
	//		if (mod16 == 0 && i != 0)
	//		{
	//			xVal = bound.min.x;
	//			yVal = bound.min.y;
	//			zVal += quadSize.z;
	//		}
	//	}
	//}

	//std::vector<int> lightsPerVoxel;
	//lightsPerVoxel.resize(bounds.size());

	//for (size_t i = 0; i < bounds.size(); ++i)
	//{
	//	for (auto point : pl)
	//	{
	//		if (bounds[i].IsInside(point.pos))
	//		{
	//			lights[i].col = point.col;
	//			lights[i].pos = point.pos;
	//			++lightsPerVoxel[i];
	//			continue;
	//		}
	//	}
	//}



	//for (size_t i = 0; i < lights.size(); ++i)
	//{
	//	if (lightsPerVoxel[i] <= 0)
	//		continue;

	//	lights[i].pos /= lightsPerVoxel[i];
	//	lights[i].col /= lightsPerVoxel[i];
	//}

}

void LGH::Recreate(const std::vector<Light>& pl)
{
	lights.clear();
	bounds.clear();

	for (auto point : pl)
	{
		bound.min.x = std::min(point.pos.x, bound.min.x);
		bound.min.y = std::min(point.pos.y, bound.min.y);
		bound.min.z = std::min(point.pos.z, bound.min.z);

		bound.max.x = std::max(point.pos.x, bound.max.x);
		bound.max.y = std::max(point.pos.y, bound.max.y);
		bound.max.z = std::max(point.pos.z, bound.max.z);
	}

	glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / (float)cube_per_axis;

	lights.resize(cube_per_axis_cubed);
	bounds.resize(cube_per_axis_cubed);

	float xVal = bound.min.x;
	float yVal = bound.min.y;
	float zVal = bound.min.z;

	int mod4;
	int mod16;

	for (size_t i = 0; i < bounds.size(); ++i)
	{
		bounds[i].min = glm::vec3(xVal, yVal, zVal);
		bounds[i].max = bounds[i].min + quadSize;

		bounds[i].center = bounds[i].min + (quadSize / 2.0f);

		xVal += quadSize.x;

		mod4 = i % cube_per_axis;
		if (mod4 == 0 && i != 0)
		{
			xVal = bound.min.x;
			yVal += quadSize.y;
			mod16 = i % cube_per_axis_squared;
			if (mod16 == 0 && i != 0)
			{
				xVal = bound.min.x;
				yVal = bound.min.y;
				zVal += quadSize.z;
			}
		}
	}

	std::vector<int> lightsPerVoxel;
	lightsPerVoxel.resize(bounds.size());

	for (size_t i = 0; i < bounds.size(); ++i)
	{
		for (auto point : pl)
		{
			if (bounds[i].IsInside(point.pos))
			{
				lights[i].col = point.col;
				lights[i].pos = point.pos;
				++lightsPerVoxel[i];
				continue;
			}
		}
		if (lightsPerVoxel[i] <= 0)
			continue;

		lights[i].pos /= lightsPerVoxel[i];
		lights[i].col /= lightsPerVoxel[i];
	}

	

	//for (size_t i = 0; i < lights.size(); ++i)
	//{
	//	if (lightsPerVoxel[i] <= 0)
	//		continue;

	//	lights[i].pos /= lightsPerVoxel[i];
	//	lights[i].col /= lightsPerVoxel[i];
	//}
}
