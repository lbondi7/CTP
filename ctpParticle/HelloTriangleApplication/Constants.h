#pragma once

#include "glm/glm.hpp"

const int WIDTH = 852;
const int HEIGHT = 480;

const int MAX_FRAMES_IN_FLIGHT = 2;

const int COMPUTE_PROCESS_NUM = 256;

struct UniformBuffer {
	alignas(16) glm::mat4 model;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct Light {
	 alignas(16) glm::vec3 col;
	 alignas(16) glm::vec3 pos;
};