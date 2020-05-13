#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <array>

#include "VkInitializer.h"

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec4 color;
	glm::vec3 constColor;
	glm::vec2 texCoord;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkInitializer::VertexBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = { 
		VkInitializer::VertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)),
		VkInitializer::VertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)),
		VkInitializer::VertexAttributeDescription(0, 2, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, color)),
		VkInitializer::VertexAttributeDescription(0, 3, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, texCoord))
		};

		return attributeDescriptions;
	}

	bool operator==(const Vertex& other) const {
		return pos == other.pos && color == other.color && texCoord == other.texCoord;
	}
};

//namespace std {
//	template<> struct hash<Vertex> {
//		size_t operator()(Vertex const& vertex) const {
//			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec4>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
//		}
//	};
//}

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return (((hash<glm::vec3>()(vertex.pos) ^ ((hash<glm::vec3>()(vertex.normal)) << 1)) ^ (hash<glm::vec4>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}