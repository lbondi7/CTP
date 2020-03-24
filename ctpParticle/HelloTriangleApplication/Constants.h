#pragma once

#include "glm/glm.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

struct Light {
	glm::vec3 col;
	glm::vec3 pos;
};