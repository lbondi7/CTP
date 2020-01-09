#pragma once

#include "VkConstants.h"

class Devices
{
public:

	Devices() = default;
	~Devices() = default;

	VkDevice& GetDevice();
	VkPhysicalDevice& GetPhysicalDevice();

private:

	VkDevice device;
	VkPhysicalDevice physicalDevice;

};

