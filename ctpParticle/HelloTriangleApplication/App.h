#pragma once

#include "VkSetup.h"
#include "Image.h"
#include "Buffer.h"
#include "Object.h"
#include "SwapChain.h"

struct ComputeStuff {
	VkQueue queue;
	VkCommandPool commandPool;
	//VkCommandBuffer commandBuffer;
	//VkFence fence;
	std::vector<VkCommandBuffer> commandBuffers;
	std::vector<VkFence> fences;
	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet descriptorSet;
	VkPipelineLayout pipelineLayout;
	VkPipeline pipeline;
	VkSemaphore semaphore;
	int32_t pipelineIndex = 0;
	uint32_t queueFamilyIndex;
};


class CTPApp {
public:

	void run();

protected:

	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	VkRenderPass renderPass;

	VkDescriptorSetLayout descriptorSetLayout;

	VkPipelineLayout pipelineLayout;

	VkSemaphore graphicsSemaphore;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	bool framebufferResized = false;

	int updateDelay = 10;

	ComputeStuff compute;

	std::vector<VkViewport> m_Viewports;
	std::vector<VkRect2D> m_ScissorRects;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	SwapChain swapchain;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkDescriptorPool descriptorPool;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<CTPApp*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void initWindow();

	void initVulkan();

	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	void createRenderPass();

	void createFramebuffers();

	void createDepthResources();

	bool hasStencilComponent(VkFormat format);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);


	void AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createSyncObjects();

	void prepareFrame(uint32_t& imageIndex);


	void endFrame(uint32_t& imageIndex);

};