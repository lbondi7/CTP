#pragma once

#include "VkSetup.h"
#include "Mesh.h"
#include "Image.h"
#include "Scene.h"
#include "Buffer.h"
#include "Texture.h"
#include "Model.h"
#include "SwapChain.h"

struct Light
{
	glm::vec3 pos;
	float radius;
};

//struct Model
//{
//	glm::vec3 pos;
//	glm::vec3 rot;
//	float scale;
//	glm::vec4 color;
//
//	static VkVertexInputBindingDescription getBindingDescription() {
//		return VkHelper::createVertexBindingDescription(1, sizeof(Model), VK_VERTEX_INPUT_RATE_INSTANCE);
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
//		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
//
//		attributeDescriptions[0] = VkHelper::createVertexAttributeDescription(1, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model, pos));
//		attributeDescriptions[1] = VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Model, rot));
//		attributeDescriptions[2] = VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32_SFLOAT, offsetof(Model, scale));
//		attributeDescriptions[3] = VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Model, color));
//
//		return attributeDescriptions;
//	}
//
//};

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

	VkRenderPass renderPass;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet particleSysDesc;

	VkPipelineLayout pipelineLayout;
	VkPipeline particleSysPipe;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;
	size_t currentFrame = 0;

	bool framebufferResized = false;

	std::vector<VkViewport> m_Viewports;
	std::vector<VkRect2D> m_ScissorRects;

	Image image;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	std::vector<InstanceData> instanceData;

	std::vector<Particle> particles;

	//Scene scene;

	Light light;

	Texture texture1;
	Texture texture2;
	Model model;
	Model model2;
	VkDescriptorSet particleSysDesc2;
	VkPipeline particleSysPipe2;

	SwapChain swapchain;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkDescriptorPool descriptorPool;
	VkDescriptorSet descriptorSet;

	float minDist = 0.0f;

	std::vector<float> minDists;

	float camSpeed = 50.0f;
	float distFromOrigin = 30.0f;
	glm::vec3 camPos = glm::vec3(0, 3.0f, -30.0f);
	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleSpeed = 2.5f;

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

	void createRenderPass();

	void createDescriptorSetLayout();

	void createDescriptorSets();

	void createGraphicsPipeline();

	void createInstances();

	glm::vec3 getFlowField(glm::vec3 pos);

	void updateInstanceBuffer();

	void createCommandBuffers();

	void createDescriptorPool();

	void createFramebuffers();

	void createDepthResources();

	bool hasStencilComponent(VkFormat format);

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image);

	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void createUniformBuffers();

	void AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	void createSyncObjects();

	void updateUniformBuffer(uint32_t currentImage);

	void drawFrame();
	void Update();
	void createLight();
	bool checkDistanceFromLight(glm::vec3 pos);
	void remapInstanceData();
	void remapVertexData();
};