#pragma once

#include "VkSetup.h"
#include "Mesh.h"
#include "Image.h"
#include "Graphics.h"
#include "Scene.h"

class CTPApp {
public:

	void run();

private:

	GLFWwindow* window;

	VkInstance instance;
	VkDebugUtilsMessengerEXT debugMessenger;
	VkSurfaceKHR surface;

	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice device;

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	SwapChainData swapChainData;
	GraphicsData graphicsData;
	CmdAndDescData cmdAndDescData;

	VkDescriptorPool descriptorPool;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet particleSysDesc;

	VkPipelineLayout pipelineLayout;
	std::vector<VkPipeline> particleSysPipe;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;


	//std::vector<Vertex> vertices;
	//std::vector<uint32_t> indices;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	//VkDescriptorPool descriptorPool;
	//std::vector<VkDescriptorSet> descriptorSets;

	//std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	bool framebufferResized = false;

	//VkBuffer stagingBuffer;
	//VkDeviceMemory stagingBufferMemory;

	std::vector<VkViewport> m_Viewports;
	std::vector<VkRect2D> m_ScissorRects;

	Graphics graphics;

	TextureData textureData;
	Mesh mesh;
	Image image;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	std::vector<InstanceData> instanceData;
	std::vector<float> lifetimes;
	std::vector<float> currentLife;

	std::vector<Particle> particles;

	Scene scene;

	float ok = 0.0f;
	int activeNum = 0;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<CTPApp*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void initWindow();

	void initVulkan();

	void mainLoop();

	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	void createDescriptorSetLayout();

	void createDescriptorSets();

	void createCommandPool();

	void createGraphicsPipeline();

	void createInstances();

	void updateInstanceBuffer();

	void createCommandBuffers();

	void createDescriptorPool();

	void createFramebuffers();

	void createDepthResources();

	bool hasStencilComponent(VkFormat format);

	void createTextureImage();

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createTextureImageView();

	void createTextureSampler();

	void createUniformBuffers();

	void createBuffers();

	void AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createSyncObjects();

	void updateUniformBuffer(uint32_t currentImage);

	void drawFrame();

};