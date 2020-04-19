#pragma once

#include "glm/glm.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Light {
	 alignas(16) glm::vec3 col;
	 alignas(16) glm::vec3 pos;
};