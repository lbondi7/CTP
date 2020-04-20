#include "LGH.h"
#include "Locator.h"
#include "ThreadManager.h"

#include <algorithm>

std::mutex lightMutex;

LGH::~LGH()
{
}

void LGH::Create(const std::vector<Light>& pl)
{
	voxels_per_axis_squared = (int)std::pow(voxels_per_axis, 2);
	voxel_per_axis_cubed = (int)std::pow(voxels_per_axis, 3);

	Recreate(&pl);
}

//void LGH::Recreate(const std::vector<Light>& pl)
//{
//	lights.clear();
//	bounds.clear();
//
//	for (auto point : pl)
//	{
//		bound.min.x = std::min(point.pos.x, bound.min.x);
//		bound.min.y = std::min(point.pos.y, bound.min.y);
//		bound.min.z = std::min(point.pos.z, bound.min.z);
//
//		bound.max.x = std::max(point.pos.x, bound.max.x);
//		bound.max.y = std::max(point.pos.y, bound.max.y);
//		bound.max.z = std::max(point.pos.z, bound.max.z);
//	}
//
//	glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / (float)voxels_per_axis;
//
//	lights.resize(voxel_per_axis_cubed);
//	bounds.resize(voxel_per_axis_cubed);
//
//	float xVal = bound.min.x;
//	float yVal = bound.min.y;
//	float zVal = bound.min.z;
//
//	int mod4;
//	int mod16;
//
//	for (size_t i = 0; i < bounds.size(); ++i)
//	{
//		bounds[i].min = glm::vec3(xVal, yVal, zVal);
//		bounds[i].max = bounds[i].min + quadSize;
//
//		bounds[i].center = bounds[i].min + (quadSize / 2.0f);
//
//		xVal += quadSize.x;
//
//		mod4 = i % voxels_per_axis;
//		if (mod4 == 0 && i != 0)
//		{
//			xVal = bound.min.x;
//			yVal += quadSize.y;
//			mod16 = i % voxels_per_axis_squared;
//			if (mod16 == 0 && i != 0)
//			{
//				xVal = bound.min.x;
//				yVal = bound.min.y;
//				zVal += quadSize.z;
//			}
//		}
//	}
//
//	std::vector<int> lightsPerVoxel;
//	lightsPerVoxel.resize(bounds.size());
//
//	for (auto point : pl)
//	{
//		for (size_t i = 0; i < bounds.size(); ++i)
//		{
//			if (bounds[i].IsInside(point.pos))
//			{
//				lights[i].col += point.col;
//				lights[i].pos += point.pos;
//				++lightsPerVoxel[i];
//				break;
//			}
//		}
//	}
//
//	for (size_t i = 0; i < bounds.size(); ++i)
//	{
//		if (lightsPerVoxel[i] <= 0)
//			continue;
//
//		lights[i].pos /= lightsPerVoxel[i];
//		lights[i].col /= lightsPerVoxel[i];
//	}
//}

void GetLightData(int start, int end, const std::vector<Light>* pl, 
	std::vector<Light>* lights, 
	std::vector<Bounds>* bounds, 
	std::vector<int>* lightsPerVoxel)
{

	auto& _pl = (*pl);
	for (int j = start; j < end; ++j)
	{
		//int i = 0;
		for (int i  = 0; i < bounds->size(); ++i)
		//for (auto& b : *bounds)
		{
			if ((*bounds)[i].IsInside(_pl[j].pos))
			{
				//std::lock_guard<std::mutex>lock(lightMutex);
				(*lights)[i].col += (*pl)[j].col;
				(*lights)[i].pos += (*pl)[j].pos;
				++(*lightsPerVoxel)[i];
				break;
			}
			//++i;
		}
	}
}

void AverageLightData(int start, int end, std::vector<Light>* lights, const std::vector<int>* lightsPerVoxel)
{
	for (size_t i = start; i < end; ++i)
	{
		if ((*lightsPerVoxel)[i] <= 0)
		{
			(*lights)[i].col = { -1.0f, -1.0f, -1.0f };
			continue;
		}
		(*lights)[i].pos /= (*lightsPerVoxel)[i];
		(*lights)[i].col /= (*lightsPerVoxel)[i];
	}
}

void LGH::Recreate(const std::vector<Light>* pl)
{
	lights.clear();
	bounds.clear();

	for (auto& point : *pl)
	{
		bound.min.x = std::min(point.pos.x, bound.min.x);
		bound.min.y = std::min(point.pos.y, bound.min.y);
		bound.min.z = std::min(point.pos.z, bound.min.z);

		bound.max.x = std::max(point.pos.x, bound.max.x);
		bound.max.y = std::max(point.pos.y, bound.max.y);
		bound.max.z = std::max(point.pos.z, bound.max.z);
	}

	glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / (float)voxels_per_axis;
	std::vector<int> lightsPerVoxel;
	lights.resize(voxel_per_axis_cubed);
	bounds.resize(voxel_per_axis_cubed);
	lightsPerVoxel.resize(voxel_per_axis_cubed);
	int start = 0;
	int end = 0;

	float xVal = bound.min.x;
	float yVal = bound.min.y;
	float zVal = bound.min.z;

	glm::vec3 quad_size = glm::vec3(bound.max.x - bound.min.x, 
		bound.max.y - bound.min.y, 
		bound.max.z - bound.min.z) / static_cast<float>(voxels_per_axis);
	int mod4;
	int mod16;

	for (size_t i = 0; i < bounds.size(); ++i)
	{
		bounds[i].min = glm::vec3(xVal, yVal, zVal);
		bounds[i].max = bounds[i].min + quad_size;

		bounds[i].center = bounds[i].min + (quad_size / 2.0f);

		xVal += quad_size.x;

		mod4 = i % voxels_per_axis;
		if (mod4 == 0 && i != 0)
		{
			xVal = bound.min.x;
			yVal += quad_size.y;
			mod16 = i % voxels_per_axis_squared;
			if (mod16 == 0 && i != 0)
			{
				xVal = bound.min.x;
				yVal = bound.min.y;
				zVal += quad_size.z;
			}
		}
	}

	int interval = pl->size() / Locator::GetThreadManager()->ThreadCount() + 1;

	end = start + interval;
	for (int i = 0; i < Locator::GetThreadManager()->ThreadCount(); ++i)
	{
		Locator::GetThreadManager()->SetTask() = std::bind(GetLightData, start, end, pl, &lights, &bounds, &lightsPerVoxel);
		start += interval;
		end = start + interval;
		if (end > pl->size())
			end = pl->size();
	}

	//Locator::GetThreadManager()->WaitForThreads();

	start = 0;
	interval = lightsPerVoxel.size() / Locator::GetThreadManager()->ThreadCount() + 1;

	end = start + interval;
	for (int i = 0; i < Locator::GetThreadManager()->ThreadCount(); ++i)
	{
		Locator::GetThreadManager()->SetTask() = std::bind(AverageLightData, start, end, &lights, &lightsPerVoxel);
		start += interval;
		end = start + interval;
		if (end > lightsPerVoxel.size())
			end = lightsPerVoxel.size();
	}
	Locator::GetThreadManager()->WaitForThreads();
}