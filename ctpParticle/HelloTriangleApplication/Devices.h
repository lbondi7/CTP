#pragma once

#include "VkConstants.h"

class Devices
{
public:

	Devices(VkPhysicalDevice _physicalDevice, VkDevice _device);
	~Devices() = default;

	VkDevice& GetDevice();
	VkPhysicalDevice& GetPhysicalDevice();



private:

	VkDevice device;
	VkPhysicalDevice physicalDevice;

};

