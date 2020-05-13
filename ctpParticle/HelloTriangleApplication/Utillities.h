#pragma once

#include "glm/glm.hpp"

#include <iostream>
#include <random>
#include <string>

struct Utillities {


	static float Dot2(const glm::vec3& v) { return glm::dot(v, v); }

	//static std::string DebugPrint(const glm::vec3& v) {
	//	 << "x: " << v.x << ", " << "y: " << v.y << ", " << "z: " << v.z << std::endl;
	//}

	static std::string V3ToString(std::string name, const glm::vec3& v) {
		return name + " | x: " + std::to_string(v.x) + ", y: " + std::to_string(v.y) + ", z: " + std::to_string(v.z);
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