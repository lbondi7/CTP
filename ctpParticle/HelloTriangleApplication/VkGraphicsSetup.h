#pragma once
#include "VkConstants.h"

#include <fstream>

namespace VkGraphicsSetup
{
	void createSwapChain(VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkDevice& device, SwapChainData& swapChainInfo, GLFWwindow* window);
	void createImageViews(const VkDevice& device, SwapChainData& swapChainData);
	void createRenderPass(const VkDevice& device, const VkPhysicalDevice& physicalDevice, const VkFormat& swapChainImageFormat, VkRenderPass& renderPass);
	void createDescriptorSetLayout(const VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
	void createGraphicsPipeline(const VkDevice& device, const VkExtent2D& swapChainExtent, GraphicsData& graphicsData);
	//VkFormat findDepthFormat(const VkPhysicalDevice& physicalDevice);
};

