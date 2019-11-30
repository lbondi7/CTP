#pragma once

#include "VkConstants.h"

namespace VkHelper
{
	void createBufferWithStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const void* data);

	void createBufferWithoutStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createBuffer(const VkDevice& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer);

	void AllocateBufferMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyMemory(const VkDevice& device, VkDeviceSize bufferSize, VkDeviceMemory& bufferMemory, const void* data);

	void copyBuffer(const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, VkCommandBufferLevel cmdBufferLevel, VkCommandPool& cmdPool, int cmdBufferCount);

	void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkDevice& device, VkCommandPool& cmdPool, int cmdBufferCount, const VkQueue graphicsQueue);

	const VkVertexInputAttributeDescription& createVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

};

