#include "Devices.h"

Devices::Devices(VkPhysicalDevice _physicalDevice, VkDevice _device)
{
	device = _device;
	physicalDevice = _physicalDevice;

}

VkDevice& Devices::GetDevice()
{
	return device;
}

VkPhysicalDevice& Devices::GetPhysicalDevice()
{
	return physicalDevice;
}
