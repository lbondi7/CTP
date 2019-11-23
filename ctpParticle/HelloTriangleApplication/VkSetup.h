#pragma once

#include "VkConstants.h"

#include <optional>
#include <vector>
#include <iostream>

namespace VkSetup
{
	void createInstance(VkInstance& instance);
	void setupDebugMessenger(const VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
	void createSurface(const VkInstance& instance, GLFWwindow* window, VkSurfaceKHR& surface);
	void pickPhysicalDevice(VkInstance& instance, VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	void createLogicalDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, VkDevice& device, VkQueue& graphicsQueue, VkQueue& presentQueue);
	void createSwapChain(VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkDevice& device, SwapChainData& swapChainInfo, GLFWwindow* window);
};

