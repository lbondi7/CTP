#pragma once

#include "glm/glm.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

struct LightShits {
	glm::vec3 col;
	float intensity;
	glm::vec3 direction;
	float diffuseIntensity;
	glm::vec3 pos;
	float constant;
	float linear;
	float exponent;
	float specIntensity;
	float specPower;
};