#pragma once

#include "VkConstants.h"

class Graphics
{
public:

	void createSwapChain(VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkDevice& device, GLFWwindow* window);

	void createImageViews(const VkDevice& device);

	void createRenderPass(const VkDevice& device, const VkPhysicalDevice& physicalDevice);

	SwapChainData& GetSwapChain();
	VkRenderPass& GetRenderPass();


private:

	VkRenderPass renderPass;
	SwapChainData swChData;
};

