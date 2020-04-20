#include "Devices.h"

#include <stdexcept>
#include <set>

Devices::Devices(VkPhysicalDevice _physicalDevice, VkDevice _device)
{
//	device = _device;
	physicalDevice = _physicalDevice;

}

VkDevice Devices::GetDevice()
{
	return device;
}

VkPhysicalDevice Devices::GetPhysicalDevice()
{
	return physicalDevice;
}

void Devices::CreateLogicalDevice(const VkSurfaceKHR& surface) {

	queueFamilyIndices = FindQueueFamilies(surface);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { queueFamilyIndices.graphicsFamily.value(), 
		queueFamilyIndices.presentFamily.value(),
	    queueFamilyIndices.computeFamily.value(), };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}

	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.geometryShader = VK_TRUE;
	deviceFeatures.fragmentStoresAndAtomics = VK_TRUE;

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	CreateCommandPool(cmdPool, Queues::GRAPHICS);
}

void Devices::CreateQueue(VkQueue& queue, Queues queueType)
{
	switch (queueType)
	{
	case Queues::GRAPHICS:
		vkGetDeviceQueue(device, queueFamilyIndices.graphicsFamily.value(), 0, &queue);
		break;
	case Queues::COMPUTE:
		//vkGetDeviceQueue(device, queueFamilyIndices.computeFamily.value(), 0, &queue);
		break;
	case Queues::PRESENT:
		vkGetDeviceQueue(device, queueFamilyIndices.presentFamily.value(), 0, &queue);
		break;
	}
}

void Devices::CreateCommandPool(VkCommandPool& commandPool, Queues queueType)
{
	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;

	std::string warning = "bobs";
	if (queueType == Queues::GRAPHICS)
	{
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
		warning = "failed to create graphics command pool!";
	}
	else if (queueType == Queues::COMPUTE)
	{
		poolInfo.queueFamilyIndex = 2;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		warning = "failed to create compute command pool!";
	}

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS) {
		throw std::runtime_error(warning);
	}
}

uint32_t Devices::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

QueueFamilyIndices Devices::QueueFamily()
{
	return queueFamilyIndices;
}

QueueFamilyIndices Devices::FindQueueFamilies(VkSurfaceKHR surface) {

	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}
		if ((queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) && (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) {
			indices.computeFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &presentSupport);

		if (presentSupport && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}

		i++;
	}

	return indices;
}

VkCommandBuffer Devices::BeginSingleTimeCommands(VkCommandBufferLevel cmdBufferLevel, VkCommandPool cmdPool, int cmdBufferCount) {

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

void Devices::EndSingleTimeCommands(VkCommandBuffer commandBuffer, VkCommandPool cmdPool, int cmdBufferCount, VkQueue queue) {

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = cmdBufferCount;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, cmdPool, cmdBufferCount, &commandBuffer);
}

VkCommandBuffer Devices::BeginSingleTimeCommands(VkCommandBufferLevel cmdBufferLevel, int cmdBufferCount) {

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

VkCommandBuffer Devices::BeginSingleTimeCommands(VkCommandBufferLevel cmdBufferLevel, int cmdBufferCount, VkCommandPool _cmdPool) {

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = cmdBufferLevel;
	allocInfo.commandPool = _cmdPool;
	allocInfo.commandBufferCount = cmdBufferCount;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);

	return commandBuffer;
}

void Devices::EndSingleTimeCommands(VkCommandBuffer commandBuffer, int cmdBufferCount, VkQueue queue) {

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = cmdBufferCount;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(device, &fenceInfo, nullptr, &fence);

	vkQueueSubmit(queue, 1, &submitInfo, fence);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, cmdPool, cmdBufferCount, &commandBuffer);
}

void Devices::EndSingleTimeCommands(VkCommandBuffer commandBuffer, int cmdBufferCount, VkCommandPool _cmdPool, VkQueue queue) {

	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = cmdBufferCount;
	submitInfo.pCommandBuffers = &commandBuffer;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	VkFence fence;
	vkCreateFence(device, &fenceInfo, nullptr, &fence);

	vkQueueSubmit(queue, 1, &submitInfo, fence);
	vkQueueWaitIdle(queue);

	vkFreeCommandBuffers(device, _cmdPool, cmdBufferCount, &commandBuffer);
}

void Devices::DestroyCommandPool() {

	vkDestroyCommandPool(device, cmdPool, nullptr);

}