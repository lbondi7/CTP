#include "Devices.h"

VkDevice& Devices::GetDevice()
{
	return device;
}

VkPhysicalDevice& Devices::GetPhysicalDevice()
{
	return physicalDevice;
}
