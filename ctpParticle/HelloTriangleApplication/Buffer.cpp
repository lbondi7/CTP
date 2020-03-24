#include "Buffer.h"
#include "Devices.h"
#include "Locator.h"

#include <memory>
#include <stdexcept>

VkResult Buffer::CreateBuffer(VkDevice _device)
{
	device = _device;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	return vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
}

void Buffer::CreateBuffer(VkBufferUsageFlags _usage, VkMemoryPropertyFlags _memProperties, VkDeviceSize _size, const void* _data)
{
	device = Locator::GetDevices()->GetDevice();
	usage = _usage;
	memProperties = _memProperties;
	size = _size;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Locator::GetDevices()->findMemoryType(memRequirements.memoryTypeBits, memProperties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	if (_data)
		CopyMem(_data);

	BindMemory();
}

void Buffer::CreateStagingBuffer(Buffer& stagingBuffer, VkDevice _device, VkBufferUsageFlags _usage)
{
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = _usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Locator::GetDevices()->GetDevice(), &bufferInfo, nullptr, &stagingBuffer.buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Locator::GetDevices()->GetDevice(), stagingBuffer.buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Locator::GetDevices()->findMemoryType(memRequirements.memoryTypeBits, memProperties);

	if (vkAllocateMemory(Locator::GetDevices()->GetDevice(), &allocInfo, nullptr, &stagingBuffer.memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(Locator::GetDevices()->GetDevice(), stagingBuffer.buffer, stagingBuffer.memory, 0);
}

Buffer Buffer::CreateStagingBuffer(VkDeviceSize _size, VkMemoryPropertyFlags _memProperties)
{
	Buffer staging;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = _size;
	bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(Locator::GetDevices()->GetDevice(), &bufferInfo, nullptr, &staging.buffer) != VK_SUCCESS) {
		throw std::runtime_error("failed to create buffer!");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(Locator::GetDevices()->GetDevice(), staging.buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Locator::GetDevices()->findMemoryType(memRequirements.memoryTypeBits, _memProperties);

	if (vkAllocateMemory(Locator::GetDevices()->GetDevice(), &allocInfo, nullptr, &staging.memory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindBufferMemory(Locator::GetDevices()->GetDevice(), staging.buffer, staging.memory, 0);

	return staging;
}

void Buffer::StageBuffer(VkDeviceSize _size, VkQueue queue, const void* _data, VkMemoryPropertyFlags _memProperties)
{
	Buffer staging = Buffer::CreateStagingBuffer(size, _memProperties);

	VkCommandBuffer cmdBuffer = Locator::GetDevices()->BeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

	//CreateStagingBuffer(staging, device, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
	vkMapMemory(device, staging.memory, 0, size, 0, &staging.data);
	memcpy(staging.data, _data, (size_t)size);
	vkUnmapMemory(device, staging.memory);
	VkBufferCopy copyRegion = {};
	copyRegion.size = size;

	vkCmdCopyBuffer(cmdBuffer, staging.buffer, buffer, 1, &copyRegion);

	Locator::GetDevices()->EndSingleTimeCommands(cmdBuffer, 1, queue);

	staging.DestoryBuffer();
}

void Buffer::Unmap(bool clearData = false)
{
	vkUnmapMemory(Locator::GetDevices()->GetDevice(), memory);
	if (clearData && data)
	{
		data = nullptr;
	}
}

VkResult Buffer::Map(VkDeviceSize _size, VkDeviceSize _offset)
{
	return vkMapMemory(Locator::GetDevices()->GetDevice(), memory, _offset, _size, 0, &data);
}

VkResult Buffer::CopyMem(const void* _data, VkDeviceSize _size, VkDeviceSize _offset)
{
	VkResult res;
	if ((res = Map(_size)) != VK_SUCCESS)
		return res;

	memcpy(data, _data, (size_t)(_size == VK_WHOLE_SIZE ? size : _size));
	Unmap();
}

//VkResult Buffer::CopyTo(void* _data, VkDeviceSize _size, VkDeviceSize _offset)
//{
//	vkMapMemory(device, memory, _offset, _size, 0, &_data);
//	memcpy(_data, data, (size_t)_size);
//	vkUnmapMemory(device, memory);
//}

VkResult Buffer::BindMemory(VkDeviceSize _offset)
{
	return vkBindBufferMemory(device, buffer, memory, _offset);
}

void Buffer::UpdateDescriptor(VkDeviceSize _range, VkDeviceSize _offset)
{
	descriptor.buffer = buffer;
	descriptor.offset = _offset;
	descriptor.range = _range;
}

void Buffer::SetupDescriptors(int amount)
{
	descriptors.resize(amount);
}

void Buffer::UpdateDescriptor(int i, VkDeviceSize _range, VkDeviceSize _offset)
{
	descriptors[i].buffer = buffer;
	descriptors[i].offset = _offset;
	descriptors[i].range = _range;
}

void Buffer::DestoryBuffer()
{
	if (buffer)
	{
		vkDestroyBuffer(Locator::GetDevices()->GetDevice(), buffer, nullptr);
	}
	if (memory)
	{
		vkFreeMemory(Locator::GetDevices()->GetDevice(), memory, nullptr);
	}
}
