#include "LGH.h"

#include <algorithm>

LGH::~LGH()
{
}

void LGH::Create(const std::vector<LightShits>& pl)
{
	for (auto point : pl)
	{
		bound.min.x = std::min(point.pos.x, bound.min.x);
		bound.min.y = std::min(point.pos.y, bound.min.y);
		bound.min.z = std::min(point.pos.z, bound.min.z);

		bound.max.x = std::max(point.pos.x, bound.max.x);
		bound.max.y = std::max(point.pos.y, bound.max.y);
		bound.max.z = std::max(point.pos.z, bound.max.z);
	}

	glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / 4.0f;

	lights.resize(4 * 4 * 4);
	bounds.resize(4 * 4 * 4);

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

		mod4 = i % 4;
		if (mod4 == 0 && i != 0)
		{
			xVal = bound.min.x;
			yVal += quadSize.y;
			mod16 = i % 16;
			if (mod16 == 0 && i != 0)
			{
				xVal = bound.min.x;
				yVal = bound.min.y;
				zVal += quadSize.z;
			}
		}
	}

	std::vector<float> lightsPerVoxel;
	lightsPerVoxel.resize(bounds.size());

	for (size_t i = 0; i < bounds.size(); ++i)
	{
		for (auto point : pl)
		{
			if (bounds[i].IsInside(point.pos))
			{
				lights[i].col += point.col;
				lights[i].pos += point.pos;
				lights[i].diffuseIntensity += point.diffuseIntensity;
				lights[i].exponent += point.exponent;
				lights[i].specIntensity += point.specIntensity;
				lights[i].specPower += point.specPower;
				lights[i].constant += point.constant;
				lights[i].intensity += point.intensity;
				lightsPerVoxel[i] += 1.0f;
			}
		}
	}

	for (size_t i = 0; i < lights.size(); ++i)
	{
		if (lightsPerVoxel[i] <= 0.0f)
			continue;

		lights[i].col /= lightsPerVoxel[i];
		lights[i].pos /= lightsPerVoxel[i];
		lights[i].diffuseIntensity /= lightsPerVoxel[i];
		lights[i].exponent /= lightsPerVoxel[i];
		lights[i].specIntensity /= lightsPerVoxel[i];
		lights[i].specPower /= lightsPerVoxel[i];
		lights[i].constant /= lightsPerVoxel[i];
		lights[i].intensity /= lightsPerVoxel[i];
	}

	int x = 0;

}

void LGH::Recreate(const std::vector<LightShits>& pl)
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

	glm::vec3 quadSize = glm::vec3(bound.max.x - bound.min.x, bound.max.y - bound.min.y, bound.max.z - bound.min.z) / 4.0f;

	lights.resize(4 * 4 * 4);
	bounds.resize(4 * 4 * 4);

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

		mod4 = i % 4;
		if (mod4 == 0 && i != 0)
		{
			xVal = bound.min.x;
			yVal += quadSize.y;
			mod16 = i % 16;
			if (mod16 == 0 && i != 0)
			{
				xVal = bound.min.x;
				yVal = bound.min.y;
				zVal += quadSize.z;
			}
		}
	}

	std::vector<float> lightsPerVoxel;
	lightsPerVoxel.resize(bounds.size());

	for (size_t i = 0; i < bounds.size(); ++i)
	{
		for (auto point : pl)
		{
			if (bounds[i].IsInside(point.pos))
			{
				lights[i].col = point.col;
				lights[i].pos = point.pos;
				lights[i].diffuseIntensity = point.diffuseIntensity;
				lights[i].exponent = point.exponent;
				lights[i].specIntensity = point.specIntensity;
				lights[i].specPower = point.specPower;
				lights[i].constant = point.constant;
				lights[i].linear = point.linear;
				lights[i].intensity = point.intensity;
				lightsPerVoxel[i] += 1.0f;
				continue;
			}
		}
	}

	

	for (size_t i = 0; i < lights.size(); ++i)
	{
		if (lightsPerVoxel[i] <= 0.0f)
			continue;

		lights[i].pos /= lightsPerVoxel[i];
		//lights[i].diffuseIntensity /= lightsPerVoxel[i];
		//lights[i].exponent /= lightsPerVoxel[i];
		//lights[i].specIntensity /= lightsPerVoxel[i];
		//lights[i].specPower /= lightsPerVoxel[i];
		//lights[i].constant /= lightsPerVoxel[i];
		//lights[i].linear /= lightsPerVoxel[i];
		//lights[i].intensity /= lightsPerVoxel[i];
	}
}
