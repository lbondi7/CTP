#include "VkHelper.h"

#include <stdexcept>

namespace VkHelper
{
	void createBufferWithStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const void* data) {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		copyMemory(device, bufferSize, stagingBufferMemory, data);

		createBuffer(device, bufferSize, usage, buffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);

		copyBuffer(device, cmdPool, graphicsQueue, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void createBufferWithoutStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

		createBuffer(device, bufferSize, usage, buffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
	}

	void createBuffer(const VkDevice& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer) {

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}
	}

	void AllocateBufferMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void copyMemory(const VkDevice& device, VkDeviceSize bufferSize, VkDeviceMemory& bufferMemory, const void* data)
	{
		void* _data;
		vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &_data);
		memcpy(_data, data, (size_t)bufferSize);
		vkUnmapMemory(device, bufferMemory);
	}

	void copyBuffer(const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdPool, 1);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer, device, cmdPool, 1, graphicsQueue);
	}

	uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, VkCommandBufferLevel cmdBufferLevel, VkCommandPool& cmdPool, int cmdBufferCount) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = cmdBufferLevel;
		allocInfo.commandPool = cmdPool;
		allocInfo.commandBufferCount = cmdBufferCount;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkDevice& device, VkCommandPool& cmdPool, int cmdBufferCount, const VkQueue graphicsQueue) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = cmdBufferCount;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, cmdPool, cmdBufferCount, &commandBuffer);
	}

};