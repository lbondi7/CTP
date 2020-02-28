#pragma once

#include "vulkan/vulkan.h"

struct Buffer
{
	VkDevice device;
	VkBuffer buffer = nullptr;
	VkDeviceMemory memory = nullptr;
	VkDeviceSize size = 0;
	VkBufferUsageFlags usage;
	VkMemoryPropertyFlags memProperties;
	VkDescriptorBufferInfo descriptor;
	void* data = nullptr;

	VkResult CreateBuffer(VkDevice _device);

	void CreateBuffer(VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memProperties, VkDeviceSize _size = VK_WHOLE_SIZE, const void* _data = nullptr);

	void CreateStagingBuffer(Buffer& stagingBuffer, VkDevice _device, VkBufferUsageFlags _usage);

	static Buffer CreateStagingBuffer(VkDeviceSize _size, VkMemoryPropertyFlags _memProperties);

	void StageBuffer(VkDeviceSize _size, VkQueue queue, const void* _data);

	void Unmap(bool clearData);

	VkResult Map(VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);

	VkResult CopyMem(const void* _data, VkDeviceSize _size = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);

	VkResult BindMemory(VkDeviceSize _offset = 0);

	void UpdateDescriptor(VkDeviceSize _range = VK_WHOLE_SIZE, VkDeviceSize _offset = 0);

	void DestoryBuffer();
};