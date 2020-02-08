#pragma once

#include "Scene.h"

//
//#include "VkSetup.h"
//#include "VkGraphicsSetup.h"
//#include "VkCmdAndDesc.h"
//#include "VkSetupHelper.h"
//#include "VkHelper.h"
//#include "Vertex.h"
//#include "Mesh.h"
//
//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//
//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>
//
//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>
//
//#include <iostream>
//#include <fstream>
//#include <stdexcept>
//#include <algorithm>
//#include <chrono>
//#include <vector>
//#include <cstring>
//#include <cstdlib>
//#include <cstdint>
//#include <array>
//#include <optional>
//#include <set>
//#include <unordered_map>
//#include <random>
//
//const int WIDTH = 800;
//const int HEIGHT = 600;
//
//const int INSTANCE_COUNT = 100;
//
//const std::string MODEL_PATH = "models/cube.obj";
//const std::string TEXTURE_PATH = "textures/chalet.jpg";
//
//const int MAX_FRAMES_IN_FLIGHT = 2;
//
//const std::vector<const char*> validationLayers = {
//	"VK_LAYER_KHRONOS_validation"
//};
//
//const std::vector<const char*> deviceExtensions = {
//	VK_KHR_SWAPCHAIN_EXTENSION_NAME
//};
//
//#ifdef NDEBUG
//const bool enableValidationLayers = false;
//#else
//const bool enableValidationLayers = true;
//#endif
//
//void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
//	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		func(instance, debugMessenger, pAllocator);
//	}
//}
//
////struct UniformBufferObject {
////	alignas(16) glm::mat4 model;
////	alignas(16) glm::mat4 view;
////	alignas(16) glm::mat4 proj;
////};
//
//struct InstanceBuffer {
//	VkBuffer buffer = VK_NULL_HANDLE;
//	VkDeviceMemory memory = VK_NULL_HANDLE;
//	size_t size = 0;
//	VkDescriptorBufferInfo descriptor;
//} instanceBuffer;
//
//struct Particle
//{
//	float maxLife;
//	float currentLife;
//	glm::vec3 velocity;
//	bool active;
//};
//
//class CTPApp {
//public:
//
//	void run() {
//		initWindow();
//		initVulkan();
//		mainLoop();
//		cleanup();
//	}
//
//private:
//	GLFWwindow* window;
//
//	VkInstance instance;
//	VkDebugUtilsMessengerEXT debugMessenger;
//	VkSurfaceKHR surface;
//
//	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
//	VkDevice device;
//
//	VkQueue graphicsQueue;
//	VkQueue presentQueue;
//
//	SwapChainData swapChainData;
//	GraphicsData graphicsData;
//	CmdAndDescData cmdAndDescData;
//
//	//VkCommandPool commandPool;
//
//	std::vector<Vertex> vertices;
//	std::vector<uint32_t> indices;
//	VkBuffer vertexBuffer;
//	VkDeviceMemory vertexBufferMemory;
//	VkBuffer indexBuffer;
//	VkDeviceMemory indexBufferMemory;
//
//	std::vector<VkBuffer> uniformBuffers;
//	std::vector<VkDeviceMemory> uniformBuffersMemory;
//
//	//VkDescriptorPool descriptorPool;
//	//std::vector<VkDescriptorSet> descriptorSets;
//
//	//std::vector<VkCommandBuffer> commandBuffers;
//
//	std::vector<VkSemaphore> imageAvailableSemaphores;
//	std::vector<VkSemaphore> renderFinishedSemaphores;
//	std::vector<VkFence> inFlightFences;
//	std::vector<VkFence> imagesInFlight;
//	size_t currentFrame = 0;
//
//	bool framebufferResized = false;
//
//	VkBuffer stagingBuffer;
//	VkDeviceMemory stagingBufferMemory;
//
//	TextureData textureData;
//	Mesh mesh;
//
//	VkImage depthImage;
//	VkDeviceMemory depthImageMemory;
//	VkImageView depthImageView;
//
//	std::vector<InstanceData> instanceData;
//	std::vector<float> lifetimes;
//	std::vector<float> currentLife;
//
//	std::vector<Particle> particles;
//
//	float ok = 0.0f;
//	int activeNum = 0;
//
//	static void framebufferResizeCallback(GLFWwindow* window, int width, int height) {
//		auto app = reinterpret_cast<CTPApp*>(glfwGetWindowUserPointer(window));
//		app->framebufferResized = true;
//	}
//
//	void initWindow() {
//		glfwInit();
//
//		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//
//		window = glfwCreateWindow(WIDTH, HEIGHT, "CTP Paticle System", nullptr, nullptr);
//		glfwSetWindowUserPointer(window, this);
//		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
//	}
//
//	void  initVulkan() {
//
//		VkSetup::createInstance(instance);
//		VkSetup::setupDebugMessenger(instance, debugMessenger);
//		VkSetup::createSurface(instance, window, surface);
//		VkSetup::pickPhysicalDevice(instance, physicalDevice, surface);
//		VkSetup::createLogicalDevice(physicalDevice, surface, device, graphicsQueue, presentQueue);
//		VkSetup::createSwapChain(physicalDevice, surface, device, swapChainData, window);
//		VkGraphicsSetup::createImageViews(device, swapChainData);
//		VkGraphicsSetup::createRenderPass(device, physicalDevice, swapChainData.swapChainImageFormat, graphicsData.renderPass);
//		VkGraphicsSetup::createDescriptorSetLayout(device, graphicsData.descriptorSetLayout);
//		VkGraphicsSetup::createGraphicsPipeline(device, swapChainData.swapChainExtent, graphicsData);
//		VkCmdAndDesc::createCommandPool(physicalDevice, device, surface, cmdAndDescData.commandPool);
//		createDepthResources();
//		createFramebuffers();
//
//		createTextureImage();
//		createTextureImageView();
//		createTextureSampler();
//		mesh.loadModel(MODEL_PATH);
//		createInstances();
//		//createVertexBuffer();
//		//createIndexBuffer();
//		//createUniformBuffers();
//		createBuffers();
//		createDescriptorPool();
//		VkCmdAndDesc::createDescriptorSets(device, cmdAndDescData, textureData, swapChainData, graphicsData, uniformBuffers);
//		//VkCmdAndDesc::createCommandBuffers(device, cmdAndDescData.commandPool, cmdAndDescData.commandBuffers, swapChainData, graphicsData, vertexBuffer, indexBuffer, cmdAndDescData.descriptorSets, mesh.indices);
//		createCommandBuffers();
//		createSyncObjects();
//	}
//
//	void  mainLoop() {
//		while (!glfwWindowShouldClose(window)) {
//			glfwPollEvents();
//			drawFrame();
//		}
//
//		vkDeviceWaitIdle(device);
//	}
//
//	void cleanup() {
//		cleanupSwapChain();
//
//		vkDestroySampler(device, textureData.textureSampler, nullptr);
//		vkDestroyImageView(device, textureData.textureImageView, nullptr);
//
//		vkDestroyImage(device, textureData.textureImage, nullptr);
//		vkFreeMemory(device, textureData.textureImageMemory, nullptr);
//
//		vkDestroyDescriptorSetLayout(device, graphicsData.descriptorSetLayout, nullptr);
//
//		vkDestroyBuffer(device, indexBuffer, nullptr);
//		vkFreeMemory(device, indexBufferMemory, nullptr);
//
//		vkDestroyBuffer(device, vertexBuffer, nullptr);
//		vkFreeMemory(device, vertexBufferMemory, nullptr);
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
//			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
//			vkDestroyFence(device, inFlightFences[i], nullptr);
//		}
//
//		vkDestroyCommandPool(device, cmdAndDescData.commandPool, nullptr);
//
//		vkDestroyDevice(device, nullptr);
//
//		if (enableValidationLayers) {
//			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
//		}
//
//		vkDestroySurfaceKHR(instance, surface, nullptr);
//		vkDestroyInstance(instance, nullptr);
//
//		glfwDestroyWindow(window);
//
//		glfwTerminate();
//	}
//
//	void recreateSwapChain() {
//		int width = 0, height = 0;
//		while (width == 0 || height == 0) {
//			glfwGetFramebufferSize(window, &width, &height);
//			glfwWaitEvents();
//		}
//
//		vkDeviceWaitIdle(device);
//
//		cleanupSwapChain();
//
//		VkGraphicsSetup::createSwapChain(physicalDevice, surface, device, swapChainData, window);
//		VkGraphicsSetup::createImageViews(device, swapChainData);
//		VkGraphicsSetup::createRenderPass(device, physicalDevice, swapChainData.swapChainImageFormat, graphicsData.renderPass);
//		VkGraphicsSetup::createGraphicsPipeline(device, swapChainData.swapChainExtent, graphicsData);
//		createDepthResources();
//		createFramebuffers();
//		createUniformBuffers();
//		createDescriptorPool();
//		VkCmdAndDesc::createDescriptorSets(device, cmdAndDescData, textureData, swapChainData, graphicsData, uniformBuffers);
//		//VkCmdAndDesc::createCommandBuffers(device, cmdAndDescData.commandPool, cmdAndDescData.commandBuffers, swapChainData, graphicsData, vertexBuffer, indexBuffer, cmdAndDescData.descriptorSets, mesh.indices);
//		createCommandBuffers();
//
//	}
//
//	void cleanupSwapChain() {
//
//		vkDestroyImageView(device, depthImageView, nullptr);
//		vkDestroyImage(device, depthImage, nullptr);
//		vkFreeMemory(device, depthImageMemory, nullptr);
//
//		for (auto framebuffer : swapChainData.swapChainFramebuffers) {
//			vkDestroyFramebuffer(device, framebuffer, nullptr);
//		}
//
//		vkFreeCommandBuffers(device, cmdAndDescData.commandPool, static_cast<uint32_t>(cmdAndDescData.commandBuffers.size()), cmdAndDescData.commandBuffers.data());
//
//		vkDestroyPipeline(device, graphicsData.graphicsPipeline, nullptr);
//		vkDestroyPipelineLayout(device, graphicsData.pipelineLayout, nullptr);
//		vkDestroyRenderPass(device, graphicsData.renderPass, nullptr);
//
//		for (auto imageView : swapChainData.swapChainImageViews) {
//			vkDestroyImageView(device, imageView, nullptr);
//		}
//
//		vkDestroySwapchainKHR(device, swapChainData.swapChain, nullptr);
//
//		for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
//			vkDestroyBuffer(device, uniformBuffers[i], nullptr);
//			vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
//		}
//
//		vkDestroyDescriptorPool(device, cmdAndDescData.descriptorPool, nullptr);
//	}
//
//	void createInstances()
//	{
//		instanceData.resize(INSTANCE_COUNT);
//		particles.resize(INSTANCE_COUNT);
//
//		std::random_device rd;
//		std::uniform_real_distribution<float> uniformDist(-3.0, 3.0f);
//		std::uniform_real_distribution<float> uniformDist2(0.0f, 3.0f);
//
//		for (auto i = 0; i < INSTANCE_COUNT; i++) {
//
//			instanceData[i].pos = glm::vec3(uniformDist(rd), uniformDist(rd), 0);
//			instanceData[i].rot = glm::vec3(uniformDist(rd), uniformDist(rd), uniformDist(rd));
//			instanceData[i].scale = 1.0f;
//			instanceData[i].texIndex = 0;
//			particles[i].maxLife = 2.0f;
//			particles[i].currentLife = 0.0f;
//			particles[i].velocity = glm::vec3(uniformDist(rd), uniformDist2(rd), uniformDist(rd));
//		}
//
//		//instanceBuffer.size = instanceData.size() * sizeof(InstanceData);
//
//		//createInstanceBuffer();
//	}
//
//	void createInstanceBuffer() {
//		VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, instanceData.data(), (size_t)bufferSize);
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, instanceBuffer.buffer, instanceBuffer.memory);
//
//		copyBuffer(stagingBuffer, instanceBuffer.buffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void updateInstanceBuffer() {
//
//		static auto startTime = std::chrono::high_resolution_clock::now();
//
//		auto currentTime = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//		ok += time;
//		if (ok > 0.5f && activeNum < INSTANCE_COUNT)
//		{
//			particles[activeNum].active = true;
//			activeNum++;
//			ok = 0.0f;
//		}
//
//		for (auto i = 0; i < INSTANCE_COUNT; i++) {
//
//			if (particles[i].active)
//			{
//				particles[i].currentLife += 0.01f;
//				if (particles[i].currentLife >= particles[i].maxLife)
//				{
//					particles[i].currentLife = 0.0f;
//					std::random_device rd;
//					std::uniform_real_distribution<float> uniformDist(-3.0, 3.0f);
//					std::uniform_real_distribution<float> uniformDist2(0.0f, 3.0f);
//					instanceData[i].pos = glm::vec3(uniformDist(rd), uniformDist(rd), 0);
//					particles[i].velocity = glm::vec3(uniformDist(rd), uniformDist2(rd), uniformDist(rd));
//				}
//				instanceData[i].pos += (particles[i].velocity * 0.1f);
//			}
//		}
//
//		VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();
//
//		copyLewisMemory(device, bufferSize, instanceBuffer.memory, instanceData.data());
//	}
//
//	void createCommandBuffers() {
//
//		cmdAndDescData.commandBuffers.resize(swapChainData.swapChainFramebuffers.size());
//
//		VkCommandBufferAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.commandPool = cmdAndDescData.commandPool;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandBufferCount = (uint32_t)cmdAndDescData.commandBuffers.size();
//
//		if (vkAllocateCommandBuffers(device, &allocInfo, cmdAndDescData.commandBuffers.data()) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate command buffers!");
//		}
//
//		for (size_t i = 0; i < cmdAndDescData.commandBuffers.size(); i++) {
//			VkCommandBufferBeginInfo beginInfo = {};
//			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//
//			if (vkBeginCommandBuffer(cmdAndDescData.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
//				throw std::runtime_error("failed to begin recording command buffer!");
//			}
//
//			VkRenderPassBeginInfo renderPassInfo = {};
//			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
//			renderPassInfo.renderPass = graphicsData.renderPass;
//			renderPassInfo.framebuffer = swapChainData.swapChainFramebuffers[i];
//			renderPassInfo.renderArea.offset = { 0, 0 };
//			renderPassInfo.renderArea.extent = swapChainData.swapChainExtent;
//
//			std::array<VkClearValue, 2> clearValues = {};
//			clearValues[0].color = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
//			clearValues[1].depthStencil = { 1.0f, 0 };
//
//			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
//			renderPassInfo.pClearValues = clearValues.data();
//
//
//			vkCmdBeginRenderPass(cmdAndDescData.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
//
//			vkCmdBindDescriptorSets(cmdAndDescData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsData.pipelineLayout, 0, 1, &cmdAndDescData.descriptorSet, 0, nullptr);
//
//			vkCmdBindPipeline(cmdAndDescData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsData.graphicsPipeline);
//
//			VkBuffer vertexBuffers[] = { vertexBuffer };
//			VkDeviceSize offsets[] = { 0 };
//			vkCmdBindVertexBuffers(cmdAndDescData.commandBuffers[i], 0, 1, &vertexBuffer, offsets);
//
//			vkCmdBindVertexBuffers(cmdAndDescData.commandBuffers[i], 1, 1, &instanceBuffer.buffer, offsets);
//
//			vkCmdBindIndexBuffer(cmdAndDescData.commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
//
//			vkCmdDrawIndexed(cmdAndDescData.commandBuffers[i], static_cast<uint32_t>(mesh.indices.size()), INSTANCE_COUNT, 0, 0, 0);
//
//			vkCmdEndRenderPass(cmdAndDescData.commandBuffers[i]);
//
//			if (vkEndCommandBuffer(cmdAndDescData.commandBuffers[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to record command buffer!");
//			}
//		}
//	}
//
//	void createDescriptorPool() {
//
//		std::array<VkDescriptorPoolSize, 2> poolSizes = {};
//		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
//		poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainData.swapChainImages.size());
//		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
//		poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainData.swapChainImages.size());
//
//		VkDescriptorPoolCreateInfo poolInfo = {};
//		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
//		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
//		poolInfo.pPoolSizes = poolSizes.data();
//		poolInfo.maxSets = static_cast<uint32_t>(swapChainData.swapChainImages.size());
//
//		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &cmdAndDescData.descriptorPool) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create descriptor pool!");
//		}
//	}
//
//	void createFramebuffers() {
//		swapChainData.swapChainFramebuffers.resize(swapChainData.swapChainImageViews.size());
//
//		for (size_t i = 0; i < swapChainData.swapChainImageViews.size(); i++) {
//
//			std::array<VkImageView, 2> attachments = {
//				swapChainData.swapChainImageViews[i],
//				depthImageView
//			};
//
//			VkFramebufferCreateInfo framebufferInfo = {};
//			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
//			framebufferInfo.renderPass = graphicsData.renderPass;
//			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
//			framebufferInfo.pAttachments = attachments.data();
//			framebufferInfo.width = swapChainData.swapChainExtent.width;
//			framebufferInfo.height = swapChainData.swapChainExtent.height;
//			framebufferInfo.layers = 1;
//
//			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainData.swapChainFramebuffers[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to create framebuffer!");
//			}
//		}
//	}
//
//	void createDepthResources() {
//
//		VkFormat depthFormat = VkSetupHelper::findDepthFormat(physicalDevice);
//
//		createImage(swapChainData.swapChainExtent.width, swapChainData.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
//		depthImageView = VkSetupHelper::createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
//
//		transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
//
//	}
//
//	bool hasStencilComponent(VkFormat format) {
//		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
//	}
//
//	void createTextureImageLewis()
//	{
//		int texWidth, texHeight, texChannels;
//		stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//		VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//		if (!pixels) {
//			throw std::runtime_error("failed to load texture image!");
//		}
//
//		createLewisBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
//		AllocateLewisBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//		copyLewisMemory(device, imageSize, stagingBufferMemory, pixels);
//
//		stbi_image_free(pixels);
//
//		createImageLewis(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureData.textureImage, textureData.textureImageMemory);
//		AllocateLewisImageMemory(physicalDevice, device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureData.textureImage, textureData.textureImageMemory);
//
//		transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//		copyBufferToImage(stagingBuffer, textureData.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//
//		transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createImageLewis(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
//		VkImageCreateInfo imageInfo = {};
//		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//		imageInfo.imageType = VK_IMAGE_TYPE_2D;
//		imageInfo.extent.width = width;
//		imageInfo.extent.height = height;
//		imageInfo.extent.depth = 1;
//		imageInfo.mipLevels = 1;
//		imageInfo.arrayLayers = 1;
//		imageInfo.format = format;
//		imageInfo.tiling = tiling;
//		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		imageInfo.usage = usage;
//		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create image!");
//		}
//	}
//
//
//	void createTextureImage()
//	{
//		int texWidth, texHeight, texChannels;
//		stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
//		VkDeviceSize imageSize = texWidth * texHeight * 4;
//
//		if (!pixels) {
//			throw std::runtime_error("failed to load texture image!");
//		}
//
//		createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, imageSize, 0, &data);
//		memcpy(data, pixels, static_cast<size_t>(imageSize));
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		stbi_image_free(pixels);
//
//		createImage(texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureData.textureImage, textureData.textureImageMemory);
//
//		transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
//		copyBufferToImage(stagingBuffer, textureData.textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
//
//		transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
//		VkImageCreateInfo imageInfo = {};
//		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//		imageInfo.imageType = VK_IMAGE_TYPE_2D;
//		imageInfo.extent.width = width;
//		imageInfo.extent.height = height;
//		imageInfo.extent.depth = 1;
//		imageInfo.mipLevels = 1;
//		imageInfo.arrayLayers = 1;
//		imageInfo.format = format;
//		imageInfo.tiling = tiling;
//		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//		imageInfo.usage = usage;
//		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create image!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetImageMemoryRequirements(device, image, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate image memory!");
//		}
//
//		vkBindImageMemory(device, image, imageMemory, 0);
//	}
//
//	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
//		VkCommandBuffer commandBuffer = beginSingleTimeCommandsLewis(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdAndDescData.commandPool, 1);
//
//		VkImageMemoryBarrier barrier = {};
//		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
//		barrier.oldLayout = oldLayout;
//		barrier.newLayout = newLayout;
//		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
//		barrier.image = image;
//		barrier.subresourceRange.baseMipLevel = 0;
//		barrier.subresourceRange.levelCount = 1;
//		barrier.subresourceRange.baseArrayLayer = 0;
//		barrier.subresourceRange.layerCount = 1;
//
//		if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
//
//			if (hasStencilComponent(format)) {
//				barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
//			}
//		}
//		else {
//			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		}
//
//		VkPipelineStageFlags sourceStage;
//		VkPipelineStageFlags destinationStage;
//
//		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
//			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
//			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
//			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//		}
//		else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
//			barrier.srcAccessMask = 0;
//			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
//
//			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
//			destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
//		}
//		else {
//			throw std::invalid_argument("unsupported layout transition!");
//		}
//
//		vkCmdPipelineBarrier(
//			commandBuffer,
//			sourceStage, destinationStage,
//			0,
//			0, nullptr,
//			0, nullptr,
//			1, &barrier
//		);
//
//		endSingleTimeCommandsLewis(commandBuffer, device, cmdAndDescData.commandPool, 1, graphicsQueue);
//	}
//
//	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
//		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
//
//		VkBufferImageCopy region = {};
//		region.bufferOffset = 0;
//		region.bufferRowLength = 0;
//		region.bufferImageHeight = 0;
//
//		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//		region.imageSubresource.mipLevel = 0;
//		region.imageSubresource.baseArrayLayer = 0;
//		region.imageSubresource.layerCount = 1;
//
//		region.imageOffset = { 0, 0, 0 };
//		region.imageExtent = {
//			width,
//			height,
//			1
//		};
//
//		vkCmdCopyBufferToImage(
//			commandBuffer,
//			buffer,
//			image,
//			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
//			1,
//			&region
//		);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	void createTextureImageView() {
//
//		textureData.textureImageView = VkSetupHelper::createImageView(device, textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
//	}
//
//	void createTextureSampler() {
//
//		VkSamplerCreateInfo samplerInfo = {};
//		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
//		samplerInfo.magFilter = VK_FILTER_LINEAR;
//		samplerInfo.minFilter = VK_FILTER_LINEAR;
//		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
//		samplerInfo.anisotropyEnable = VK_FALSE;
//		samplerInfo.maxAnisotropy = 1;
//		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
//		samplerInfo.unnormalizedCoordinates = VK_FALSE;
//		samplerInfo.compareEnable = VK_FALSE;
//		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
//		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
//		samplerInfo.mipLodBias = 0.0f;
//		samplerInfo.minLod = 0.0f;
//		samplerInfo.maxLod = 0.0f;
//
//		if (vkCreateSampler(device, &samplerInfo, nullptr, &textureData.textureSampler) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create texture sampler!");
//		}
//	}
//
//	void createVertexBuffer() {
//		VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, mesh.vertices.data(), (size_t)bufferSize);
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
//
//		copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createIndexBuffer() {
//		VkDeviceSize bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		void* data;
//		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
//		memcpy(data, mesh.indices.data(), (size_t)bufferSize);
//		vkUnmapMemory(device, stagingBufferMemory);
//
//		createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);
//
//		copyBuffer(stagingBuffer, indexBuffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createBufferWithStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const void* data) {
//
//		VkBuffer stagingBuffer;
//		VkDeviceMemory stagingBufferMemory;
//		createLewisBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
//		AllocateLewisBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
//
//		copyLewisMemory(device, bufferSize, stagingBufferMemory, data);
//
//		createLewisBuffer(device, bufferSize, usage, buffer);
//		AllocateLewisBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
//
//		copyBufferLewis(device, cmdPool, graphicsQueue, stagingBuffer, buffer, bufferSize);
//
//		vkDestroyBuffer(device, stagingBuffer, nullptr);
//		vkFreeMemory(device, stagingBufferMemory, nullptr);
//	}
//
//	void createUniformBuffers() {
//		VkDeviceSize bufferSize = sizeof(UniformBufferObject);
//
//		uniformBuffers.resize(swapChainData.swapChainImages.size());
//		uniformBuffersMemory.resize(swapChainData.swapChainImages.size());
//
//		for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
//			createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
//		}
//	}
//
//	void createBuffers()
//	{
//		VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();
//
//		createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool,
//			graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//			bufferSize, instanceBuffer.buffer, instanceBuffer.memory, instanceData.data());
//
//		bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();
//
//		createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool,
//			graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
//			bufferSize, vertexBuffer, vertexBufferMemory, mesh.vertices.data());
//
//		bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();
//
//		createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool, 
//			graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, 
//			bufferSize, indexBuffer, indexBufferMemory, mesh.indices.data());
//
//		bufferSize = sizeof(UniformBufferObject);
//
//		uniformBuffers.resize(swapChainData.swapChainImages.size());
//		uniformBuffersMemory.resize(swapChainData.swapChainImages.size());
//
//		for (size_t i = 0; i < swapChainData.swapChainImages.size(); i++) {
//			VkHelper::createBufferWithoutStaging(physicalDevice, device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
//				bufferSize, uniformBuffers[i], uniformBuffersMemory[i]);
//		}
//	}
//
//	void createLewisBuffer(const VkDevice& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer) {
//		
//		VkBufferCreateInfo bufferInfo = {};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = bufferSize;
//		bufferInfo.usage = usage;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create buffer!");
//		}
//	}
//
//	void AllocateLewisBufferMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
//	{
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryTypeLewis(physicalDevice, memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(device, buffer, bufferMemory, 0);
//	}
//
//	void AllocateLewisImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
//	{
//		VkMemoryRequirements memRequirements;
//		vkGetImageMemoryRequirements(device, image, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryTypeLewis(physicalDevice, memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindImageMemory(device, image, imageMemory, 0);
//	}
//
//
//	void copyLewisMemory(const VkDevice& device, VkDeviceSize bufferSize, VkDeviceMemory& bufferMemory, const void* data)
//	{
//		void* _data;
//		vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &_data);
//		memcpy(_data, data, (size_t)bufferSize);
//		vkUnmapMemory(device, bufferMemory);
//	}
//
//	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
//		VkBufferCreateInfo bufferInfo = {};
//		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
//		bufferInfo.size = size;
//		bufferInfo.usage = usage;
//		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
//
//		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
//			throw std::runtime_error("failed to create buffer!");
//		}
//
//		VkMemoryRequirements memRequirements;
//		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);
//
//		VkMemoryAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		allocInfo.allocationSize = memRequirements.size;
//		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);
//
//		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
//			throw std::runtime_error("failed to allocate buffer memory!");
//		}
//
//		vkBindBufferMemory(device, buffer, bufferMemory, 0);
//	}
//
//	void copyBufferLewis(const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
//		VkCommandBuffer commandBuffer = beginSingleTimeCommandsLewis(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdPool, 1);
//
//		VkBufferCopy copyRegion = {};
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//		endSingleTimeCommandsLewis(commandBuffer, device, cmdPool, 1, graphicsQueue);
//	}
//
//	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
//		VkCommandBuffer commandBuffer = beginSingleTimeCommands();
//
//		VkBufferCopy copyRegion = {};
//		copyRegion.size = size;
//		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
//
//		endSingleTimeCommands(commandBuffer);
//	}
//
//	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//		VkPhysicalDeviceMemoryProperties memProperties;
//		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//				return i;
//			}
//		}
//
//		throw std::runtime_error("failed to find suitable memory type!");
//	}
//
//	uint32_t findMemoryTypeLewis(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
//		VkPhysicalDeviceMemoryProperties memProperties;
//		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
//
//		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
//			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
//				return i;
//			}
//		}
//
//		throw std::runtime_error("failed to find suitable memory type!");
//	}
//
//	void createSyncObjects() {
//		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
//		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
//		imagesInFlight.resize(swapChainData.swapChainImages.size(), VK_NULL_HANDLE);
//
//		VkSemaphoreCreateInfo semaphoreInfo = {};
//		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
//
//		VkFenceCreateInfo fenceInfo = {};
//		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
//		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
//
//		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
//			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
//				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
//				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
//				throw std::runtime_error("failed to create synchronization objects for a frame!");
//			}
//		}
//	}
//
//	void updateUniformBuffer(uint32_t currentImage) {
//		static auto startTime = std::chrono::high_resolution_clock::now();
//
//		auto currentTime = std::chrono::high_resolution_clock::now();
//		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
//
//		UniformBufferObject ubo = {};
//		ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(time * glm::radians(90.0f), 1.0f, time * glm::radians(90.0f)));
//		ubo.view = glm::lookAt(glm::vec3(0.0f, 3.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		ubo.proj = glm::perspective(glm::radians(45.0f), swapChainData.swapChainExtent.width / (float)swapChainData.swapChainExtent.height, 0.1f, 1000.0f);
//		ubo.proj[1][1] *= -1;
//
//		void* data;
//		vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
//		memcpy(data, &ubo, sizeof(ubo));
//		vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
//	}
//
//	VkCommandBuffer beginSingleTimeCommandsLewis(const VkDevice& device, VkCommandBufferLevel cmdBufferLevel, VkCommandPool& cmdPool, int cmdBufferCount) {
//		VkCommandBufferAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.level = cmdBufferLevel;
//		allocInfo.commandPool = cmdPool;
//		allocInfo.commandBufferCount = cmdBufferCount;
//
//		VkCommandBuffer commandBuffer;
//		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//		VkCommandBufferBeginInfo beginInfo = {};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//		return commandBuffer;
//	}
//
//	void endSingleTimeCommandsLewis(VkCommandBuffer commandBuffer, const VkDevice& device, VkCommandPool& cmdPool, int cmdBufferCount, const VkQueue graphicsQueue) {
//		vkEndCommandBuffer(commandBuffer);
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = cmdBufferCount;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(graphicsQueue);
//
//		vkFreeCommandBuffers(device, cmdPool, cmdBufferCount, &commandBuffer);
//	}
//
//	VkCommandBuffer beginSingleTimeCommands() {
//		VkCommandBufferAllocateInfo allocInfo = {};
//		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
//		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
//		allocInfo.commandPool = cmdAndDescData.commandPool;
//		allocInfo.commandBufferCount = 1;
//
//		VkCommandBuffer commandBuffer;
//		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);
//
//		VkCommandBufferBeginInfo beginInfo = {};
//		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
//		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
//
//		vkBeginCommandBuffer(commandBuffer, &beginInfo);
//
//		return commandBuffer;
//	}
//
//	void endSingleTimeCommands(VkCommandBuffer commandBuffer) {
//		vkEndCommandBuffer(commandBuffer);
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &commandBuffer;
//
//		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
//		vkQueueWaitIdle(graphicsQueue);
//
//		vkFreeCommandBuffers(device, cmdAndDescData.commandPool, 1, &commandBuffer);
//	}
//
//	void drawFrame() {
//		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
//
//		uint32_t imageIndex;
//		VkResult result = vkAcquireNextImageKHR(device, swapChainData.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);
//
//		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
//			recreateSwapChain();
//			return;
//		}
//		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
//			throw std::runtime_error("failed to acquire swap chain image!");
//		}
//
//		updateInstanceBuffer();
//		updateUniformBuffer(imageIndex);
//
//		if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
//			vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
//		}
//		imagesInFlight[imageIndex] = inFlightFences[currentFrame];
//
//		VkSubmitInfo submitInfo = {};
//		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
//
//		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
//		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
//		submitInfo.waitSemaphoreCount = 1;
//		submitInfo.pWaitSemaphores = waitSemaphores;
//		submitInfo.pWaitDstStageMask = waitStages;
//		submitInfo.commandBufferCount = 1;
//		submitInfo.pCommandBuffers = &cmdAndDescData.commandBuffers[imageIndex];
//
//		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
//		submitInfo.signalSemaphoreCount = 1;
//		submitInfo.pSignalSemaphores = signalSemaphores;
//
//		vkResetFences(device, 1, &inFlightFences[currentFrame]);
//
//		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
//			throw std::runtime_error("failed to submit draw command buffer!");
//		}
//
//		VkPresentInfoKHR presentInfo = {};
//		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//
//		presentInfo.waitSemaphoreCount = 1;
//		presentInfo.pWaitSemaphores = signalSemaphores;
//
//		VkSwapchainKHR swapChains[] = { swapChainData.swapChain };
//		presentInfo.swapchainCount = 1;
//		presentInfo.pSwapchains = swapChains;
//
//		presentInfo.pImageIndices = &imageIndex;
//
//		result = vkQueuePresentKHR(presentQueue, &presentInfo);
//
//		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
//			framebufferResized = false;
//			recreateSwapChain();
//		}
//		else if (result != VK_SUCCESS) {
//			throw std::runtime_error("failed to present swap chain image!");
//		}
//
//		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
//	}
//
//};

int main() {

	Scene app;

	try {
		app.run();
	}
	catch (const std::exception & e) {
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}