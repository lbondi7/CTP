#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <array>

#include "VkHelper.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 color;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions = {};

		attributeDescriptions[0] = VkHelper::createVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos));
		attributeDescriptions[1] = VkHelper::createVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, color));
		attributeDescriptions[2] = VkHelper::createVertexAttributeDescription(0, 2, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord));

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^
				(hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
				(hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

struct InstanceData {
	glm::vec3 pos;
	glm::vec3 rot;
	float scale;
	uint32_t texIndex;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(1, sizeof(InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE);
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

		attributeDescriptions[0] = VkHelper::createVertexAttributeDescription(1, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceData, pos));
		attributeDescriptions[1] = VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceData, rot));
		attributeDescriptions[2] = VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32_SFLOAT, offsetof(InstanceData, scale));
		attributeDescriptions[3] = VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32_SINT, offsetof(InstanceData, texIndex));

		return attributeDescriptions;
	}
};