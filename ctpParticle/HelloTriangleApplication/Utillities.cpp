#include "Utillities.h"


std::random_device Utillities::rd;

int Utillities::GetRandomInt(int min, int max)
{
	std::uniform_int_distribution<int> rand(min, max);
	return rand(rd);
}

float Utillities::GetRandomFloat(float min, float max)
{
	std::uniform_real_distribution<float> rand(min, max);
	return rand(rd);
}

glm::vec3 Utillities::GetRandomVec3(float min, float max)
{
	if (max < min)
		std::swap(min, max);

	std::uniform_real_distribution<float> rand(min, max);

	return glm::vec3(rand(rd), rand(rd), rand(rd));
}

glm::vec3 Utillities::GetRandomVec3(const glm::vec3& min, const glm::vec3& max)
{
	if (max.x < min.x || max.y < min.y || max.z < min.z)
		return glm::vec3(0.0f);

	std::uniform_real_distribution<float> randX(min.x, max.x);
	std::uniform_real_distribution<float> randY(min.y, max.y);
	std::uniform_real_distribution<float> randZ(min.z, max.z);

	return glm::vec3(randX(rd), randY(rd), randZ(rd));
}
