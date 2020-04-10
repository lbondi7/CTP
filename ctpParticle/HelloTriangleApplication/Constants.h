#pragma once

#include "glm/glm.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

struct Light {
	alignas(16) glm::vec3 col;
	alignas(16) glm::vec3 pos;
};