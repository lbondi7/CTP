#pragma once

#include "VkSetup.h"
#include "Image.h"
#include "Buffer.h"
#include "Object.h"
#include "SwapChain.h"



struct Light
{
	glm::vec3 pos;
	float radius;
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

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<VkViewport> m_Viewports;
	std::vector<VkRect2D> m_ScissorRects;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	//Scene scene;

	Light light;

	SwapChain swapchain;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkDescriptorPool descriptorPool;

	VkDescriptorSet objDesc;
	std::vector<Object> objs;
	std::vector<VkDescriptorSet> objDescs;

	//Object object;
	//VkDescriptorSet objectDescSet;
	//VkPipeline objectPipeline;

	//Vertex point;
	//std::vector<Buffer> uniformPoint;
	//Buffer vertex;
	//VkDescriptorSet pointDescSet;
	//VkPipeline pointPipeline;
	//Texture pointTexture;

	float minDist = 0.0f;

	std::vector<float> minDists;

	//float camSpeed = 50.0f;
	//float distFromOrigin = 30.0f;
	//glm::vec3 camPos = glm::vec3(0, 3.0f, -30.0f);
	//float angleX = 0.0f;
	//float angleY = 0.0f;
	//float angleSpeed = 2.5f;

	float ok = 0.0f;
	int activeNum = 0;

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
		auto app = reinterpret_cast<CTPApp*>(glfwGetWindowUserPointer(window));
		app->framebufferResized = true;
	}

	void initWindow();

	void initVulkan();

//	void mainLoop();

	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	void createRenderPass();

	//void createDescriptorSetLayout();

//	void createDescriptorSets();

//	void createGraphicsPipeline();

	//glm::vec3 getFlowField(glm::vec3 pos);

	//void createCommandBuffers();

//	void createDescriptorPool();

	void createFramebuffers();

	void createDepthResources();

	bool hasStencilComponent(VkFormat format);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

//	void createUniformBuffers();

	void AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createSyncObjects();

	void prepareFrame(uint32_t& imageIndex);

	void updateUniformBuffer(uint32_t currentImage);

	//void drawFrame();
	void endFrame(uint32_t& imageIndex);
	//void Update();
	void createLight();
	bool checkDistanceFromLight(glm::vec3 pos);

	//void LoadAssets();

};