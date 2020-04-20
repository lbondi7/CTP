#pragma once

#include "glm/glm.hpp"

#include <iostream>
#include <random>

struct Utillities {


	static float Dot2(const glm::vec3& v) { return glm::dot(v, v); }

	static void DebugPrint(const glm::vec3& v) {
		std::cout << "x: " << v.x << ", " << "y: " << v.y << ", " << "z: " << v.z << std::endl;
	}

	static void DebugPrint(const char* word) {
		std::cout << word << std::endl;
	}

	static int GetRandomInt(int min, int max);

	static float GetRandomFloat(float min, float max);

	static glm::vec3 GetRandomVec3(float min, float max);

	static glm::vec3 GetRandomVec3(const glm::vec3& min, const glm::vec3& max);

private:
	static std::random_device rd;

};