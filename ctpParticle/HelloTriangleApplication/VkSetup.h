#pragma once

#include "VkConstants.h"

#include <optional>
#include <vector>
#include <iostream>


struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

class VkSetup
{

public:

	static void createInstance(VkInstance& instance);
	static void setupDebugMessenger(const VkInstance& instance, VkDebugUtilsMessengerEXT& debugMessenger);
	static void createSurface(const VkInstance& instance, GLFWwindow* window, VkSurfaceKHR& surface);
	static void pickPhysicalDevice(VkInstance& instance, VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	static void createLogicalDevice(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, VkDevice& device, VkQueue& graphicsQueue, VkQueue& presentQueue);
	static void createSwapChain(VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface, const VkDevice& device, SwapChainData& swapChainInfo, GLFWwindow* window);


	static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	static SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	static bool checkDeviceExtensionSupport(const VkPhysicalDevice& physicalDevice);

private:

	static bool isDeviceSuitable(const VkPhysicalDevice& physicalDevice, const VkSurfaceKHR& surface);
	static std::vector<const char*> getRequiredExtensions();
	static bool checkValidationLayerSupport();
	static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
};

namespace Hello
{

};

