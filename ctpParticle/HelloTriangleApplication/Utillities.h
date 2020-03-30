#pragma once

#include "glm/glm.hpp"

#include <iostream>

namespace Utillities {


	float Dot2(const glm::vec3& v) { return glm::dot(v, v); }

	void DebugPrint(const glm::vec3& v) {
		std::cout << "x: " << v.x << ", " << "y: " << v.y << ", " << "z: " << v.z << std::endl;
	}

	void DebugPrint(const char* word) {
		std::cout << word << std::endl;
	}

}