#pragma once

#include "VkConstants.h"
//#include "Buffer.h"

class Devices
{
public:

	Devices(VkPhysicalDevice _physicalDevice, VkDevice _device);
	~Devices() = default;

	VkDevice GetDevice();
	VkPhysicalDevice GetPhysicalDevice();

	void CreateLogicalDevice(const VkSurfaceKHR& surface);

	void CreateQueue(VkQueue& queue, Queues queueType);

	void CreateCommandPool(VkCommandPool& commandPool);

	QueueFamilyIndices FindQueueFamilies(VkSurfaceKHR surface);

	VkCommandBuffer BeginSingleTimeCommands(VkCommandBufferLevel cmdBufferLevel, VkCommandPool cmdPool, int cmdBufferCount);

	VkCommandBuffer BeginSingleTimeCommands(VkCommandBufferLevel cmdBufferLevel, int cmdBufferCount);

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer, int cmdBufferCount, VkQueue queue);

	void DestroyCommandPool();

	void EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool cmdPool, int cmdBufferCount, VkQueue queue);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:

	VkDevice device;
	VkPhysicalDevice physicalDevice;
	VkCommandPool cmdPool;

	QueueFamilyIndices queueFamilyIndices;
};
