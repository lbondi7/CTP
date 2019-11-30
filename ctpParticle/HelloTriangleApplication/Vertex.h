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
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
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
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 1;
		bindingDescription.stride = sizeof(InstanceData);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};


		attributeDescriptions[0] = VkHelper::createVertexAttributeDescription(1, 3, VK_FORMAT_R32G32B32_SFLOAT, 0);
		attributeDescriptions[1] = VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3);
		attributeDescriptions[2] = VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32_SFLOAT, sizeof(float) * 6);
		attributeDescriptions[3] = VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32_SINT, sizeof(float) * 7);

		//attributeDescriptions[0].binding = 1;
		//attributeDescriptions[0].location = 3;
		//attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		//attributeDescriptions[0].offset = 0;

		//attributeDescriptions[1].binding = 1;
		//attributeDescriptions[1].location = 4;
		//attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//attributeDescriptions[1].offset = sizeof(float) * 3;

		//attributeDescriptions[2].binding = 1;
		//attributeDescriptions[2].location = 5;
		//attributeDescriptions[2].format = VK_FORMAT_R32_SFLOAT;
		//attributeDescriptions[2].offset = sizeof(float) * 6;

		//attributeDescriptions[3].binding = 1;
		//attributeDescriptions[3].location = 6;
		//attributeDescriptions[3].format = VK_FORMAT_R32_SINT;
		//attributeDescriptions[3].offset = sizeof(float) * 7;

		return attributeDescriptions;
	}
};

//namespace extraFunc
//{
//
//	VkVertexInputAttributeDescription vertexInputAttributeDescription(
//		uint32_t binding,
//		uint32_t location,
//		VkFormat format,
//		uint32_t offset)
//	{
//		VkVertexInputAttributeDescription vInputAttribDescription{};
//		vInputAttribDescription.location = location;
//		vInputAttribDescription.binding = binding;
//		vInputAttribDescription.format = format;
//		vInputAttribDescription.offset = offset;
//		return vInputAttribDescription;
//	}
//}