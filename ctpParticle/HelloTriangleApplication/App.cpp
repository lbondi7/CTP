#pragma once
#include "App.h"

#include "VkGraphicsSetup.h"
#include "VkCmdAndDesc.h"
#include "VkSetupHelper.h"
#include "VkHelper.h"
#include "Locator.h"
#include "Timer.h"
#include "Devices.h"
#include "Keyboard.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include <unordered_map>
#include <random>

const int WIDTH = 800;
const int HEIGHT = 600;

const int INSTANCE_COUNT = 1000;

const std::string MODEL_PATH = "../../../Models/sphere.obj";
const std::string TEXTURE_PATH = "textures/orangeSkin.jpg";

const int MAX_FRAMES_IN_FLIGHT = 2;

//const std::vector<const char*> validationLayers = {
//	"VK_LAYER_KHRONOS_validation"
//};
//
//const std::vector<const char*> deviceExtensions = {
//	VK_KHR_SWAPCHAIN_EXTENSION_NAME
//};

//void CTPApp::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
//	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
//	if (func != nullptr) {
//		func(instance, debugMessenger, pAllocator);
//	}
//}

//struct UniformBufferObject {
//	alignas(16) glm::mat4 model;
//	alignas(16) glm::mat4 view;
//	alignas(16) glm::mat4 proj;
//};

struct InstanceBuffer {
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	size_t size = 0;
	VkDescriptorBufferInfo descriptor;
} instanceBuffer;

void CTPApp::run() {
	initWindow();
	//Locator::InitDevices(new Devices());
	initVulkan();
	Locator::InitTimer(new Timer());
	Locator::InitKeyboard(new Keyboard(window));
	mainLoop();
	cleanup();
}

void CTPApp::initWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	window = glfwCreateWindow(WIDTH, HEIGHT, "CTP Paticle System", nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

}

void CTPApp::initVulkan() {

	VkSetup::createInstance(instance);
	VkSetup::setupDebugMessenger(instance, debugMessenger);
	VkSetup::createSurface(instance, window, surface);
	VkSetup::pickPhysicalDevice(instance, physicalDevice, surface);
	VkSetup::createLogicalDevice(physicalDevice, surface, device, graphicsQueue, presentQueue);
	graphics.createSwapChain(physicalDevice, surface, device, window);
	graphics.createImageViews(device);
	graphics.createRenderPass(device, physicalDevice);
	createDescriptorSetLayout();
	createCommandPool();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();

	image.loadImage(TEXTURE_PATH.c_str());
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	mesh.loadModel(MODEL_PATH.c_str());
	createInstances();

	scene.Init(&physicalDevice, &device, window, &graphicsQueue, &presentQueue, &graphics);
	scene.CreateUniformBuffers();
	createBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
	//scene.CreateUniformBuffers();
}

void CTPApp::mainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Locator::GetTimer()->GetTimePoint();
		drawFrame();
	}

	vkDeviceWaitIdle(device);
}

void CTPApp::cleanup() {
	cleanupSwapChain();

	vkDestroySampler(device, textureData.textureSampler, nullptr);
	vkDestroyImageView(device, textureData.textureImageView, nullptr);

	vkDestroyImage(device, textureData.textureImage, nullptr);
	vkFreeMemory(device, textureData.textureImageMemory, nullptr);

	vkDestroyDescriptorSetLayout(device, graphicsData.descriptorSetLayout, nullptr);

	vkDestroyBuffer(device, indexBuffer, nullptr);
	vkFreeMemory(device, indexBufferMemory, nullptr);

	vkDestroyBuffer(device, vertexBuffer, nullptr);
	vkFreeMemory(device, vertexBufferMemory, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, cmdAndDescData.commandPool, nullptr);

	vkDestroyDevice(device, nullptr);

	if (enableValidationLayers) {
		VkSetup::DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
	}

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);

	glfwDestroyWindow(window);

	glfwTerminate();
}

void CTPApp::recreateSwapChain() {
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	cleanupSwapChain();

	graphics.createSwapChain(physicalDevice, surface, device, window);
	graphics.createImageViews(device);
	graphics.createRenderPass(device, physicalDevice);
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	//createUniformBuffers();
	scene.CreateUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();

}

void CTPApp::cleanupSwapChain() {

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : graphics.GetSwapChain().swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkFreeCommandBuffers(device, cmdAndDescData.commandPool, static_cast<uint32_t>(cmdAndDescData.commandBuffers.size()), cmdAndDescData.commandBuffers.data());

	vkDestroyPipeline(device, graphicsData.graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, graphicsData.pipelineLayout, nullptr);
	vkDestroyRenderPass(device, graphics.GetRenderPass(), nullptr);

	for (auto imageView : graphics.GetSwapChain().swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, graphics.GetSwapChain().swapChain, nullptr);

	//for (size_t i = 0; i < graphics.GetSwapChain().swapChainImages.size(); i++) {
	//	vkDestroyBuffer(device, uniformBuffers[i], nullptr);
	//	vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	//}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void CTPApp::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkHelper::createDescriptorLayoputBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT);

	VkDescriptorSetLayoutBinding samplerLayoutBinding = VkHelper::createDescriptorLayoputBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = VkHelper::createDescSetLayoutInfo(static_cast<uint32_t>(bindings.size()), bindings.data());

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &graphicsData.descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}

void CTPApp::createDescriptorSets() {

	std::vector<VkDescriptorSetLayout> layouts(graphics.GetSwapChain().swapChainImages.size(), graphicsData.descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	//particleSysDescs.resize(graphics.GetSwapChain().swapChainImages.size());
	if (vkAllocateDescriptorSets(device, &allocInfo, &particleSysDesc) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < graphics.GetSwapChain().swapChainImages.size(); i++) {

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = scene.uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageInfo.imageView = textureData.textureImageView;
		imageInfo.sampler = textureData.textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {
		descriptorWrites[0] = VkHelper::writeDescSet(particleSysDesc, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
				descriptorWrites[1] = VkHelper::writeDescSet(particleSysDesc, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &imageInfo)
		};

		//descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[0].dstSet = particleSysDesc;
		//descriptorWrites[0].dstBinding = 0;
		//descriptorWrites[0].dstArrayElement = 0;
		//descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//descriptorWrites[0].descriptorCount = 1;
		//descriptorWrites[0].pBufferInfo = &bufferInfo;

		//descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		//descriptorWrites[1].dstSet = particleSysDesc;
		//descriptorWrites[1].dstBinding = 1;
		//descriptorWrites[1].dstArrayElement = 0;
		//descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//descriptorWrites[1].descriptorCount = 1;
		//descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}

void CTPApp::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = VkSetupHelper::findQueueFamilies(physicalDevice, surface);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &cmdAndDescData.commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics command pool!");
	}
}

void CTPApp::createGraphicsPipeline() {
	auto vertShaderCode = VkSetupHelper::readFile("shaders/basic/basicVert.spv");
	auto fragShaderCode = VkSetupHelper::readFile("shaders/basic/basicFrag.spv");

	VkShaderModule vertShaderModule = VkSetupHelper::createShaderModule(vertShaderCode, device);
	VkShaderModule fragShaderModule = VkSetupHelper::createShaderModule(fragShaderCode, device);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = VkHelper::createShaderStageInfo(
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		VK_SHADER_STAGE_VERTEX_BIT,
		vertShaderModule,
		"main");

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = VkHelper::createShaderStageInfo(
		VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
		VK_SHADER_STAGE_FRAGMENT_BIT,
		fragShaderModule,
		"main");

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	bindingDescriptions = {
		Vertex::getBindingDescription(),
		InstanceData::getBindingDescription()
	};

	attributeDescriptions = {
		Vertex::getAttributeDescriptions()[0],
		Vertex::getAttributeDescriptions()[1],
		Vertex::getAttributeDescriptions()[2],

		InstanceData::getAttributeDescriptions()[0],
		InstanceData::getAttributeDescriptions()[1],
		InstanceData::getAttributeDescriptions()[2],
		InstanceData::getAttributeDescriptions()[3] };

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = VkHelper::createViewport(0.0f, 0.0f, (float)graphics.GetSwapChain().swapChainExtent.width, (float)graphics.GetSwapChain().swapChainExtent.height, 0.0f, 1.0f);

	//VkViewport viewport = VkHelper::createViewport(0, 0, graphics.GetSwapChain().swapChainExtent.width, graphics.GetSwapChain().swapChainExtent.height, 0.0f, 1.0f);
	VkRect2D scissor = VkHelper::createScissorRect({ 0, 0 }, graphics.GetSwapChain().swapChainExtent);

	VkPipelineViewportStateCreateInfo viewportState = VkHelper::createViewPortStateInfo(1, 1, &viewport, &scissor);

	VkPipelineRasterizationStateCreateInfo rasterizer = VkHelper::createRasteriser(
		VK_FALSE, VK_FALSE,
		VK_POLYGON_MODE_FILL, 1.0f,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_FALSE);

	VkPipelineMultisampleStateCreateInfo multisampling = 
		VkHelper::createMultiSampling(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

	VkPipelineColorBlendAttachmentState colorBlendAttachment = 
		VkHelper::createColourBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);

	VkPipelineColorBlendStateCreateInfo colorBlending = VkHelper::createColourBlendStateInfo(
		VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment, { 0.0f, 0.0f, 0.0f, 0.0f });

	VkPipelineDepthStencilStateCreateInfo depthStencil = VkHelper::createDepthStencilInfo(
		VK_TRUE, VK_TRUE,
		VK_COMPARE_OP_LESS,
		VK_FALSE, VK_FALSE);

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &graphicsData.descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &graphicsData.pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = graphicsData.pipelineLayout;
	pipelineInfo.renderPass = graphics.GetRenderPass();
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsData.graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(device, fragShaderModule, nullptr);
	vkDestroyShaderModule(device, vertShaderModule, nullptr);
}

void CTPApp::createInstances()
{
	instanceData.resize(INSTANCE_COUNT);
	particles.resize(INSTANCE_COUNT);

	std::random_device rd;
	std::uniform_real_distribution<float> uniformDist(-1.0, 1.0f);
	std::uniform_real_distribution<float> uniformDist2(0.0f, 3.0f);
	std::uniform_real_distribution<float> randStartPos(-30.0f, 30.0f);

	for (auto i = 0; i < INSTANCE_COUNT; i++) {

		instanceData[i].pos = glm::vec3(randStartPos(rd), randStartPos(rd), randStartPos(rd));
		instanceData[i].rot = glm::vec3(uniformDist(rd), uniformDist(rd), uniformDist(rd));
		instanceData[i].scale = 1.0f;
		instanceData[i].texIndex = 0;
		particles[i].maxLife = 1.0f;
		particles[i].currentLife = 0.0f;
		particles[i].velocity = glm::vec3(0, 0, 0);
		particles[i].active = true;
		randVels.push_back(glm::vec3(0, 0, 0));
	}

	//instanceBuffer.size = instanceData.size() * sizeof(InstanceData);

	//createInstanceBuffer();
}

glm::vec3 CTPApp::getFlowField(glm::vec3 pos)
{
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, -pos.x * pos.y) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z)));
	glm::vec3 vel = (glm::vec3(-pos.y, pos.x, pos.x) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z)));
	//vel.x = std::sqrt((pos.x * pos.x) + (pos.y * pos.y));
	//vel.y = 0;
	//vel.z = 0;

	return vel;
}

void CTPApp::updateInstanceBuffer() {

	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	auto delTime = Locator::GetTimer()->DeltaTime();
	//std::cout << delTime << std::endl;
	//ok += delTime;
	//if (ok > 0.1f && activeNum < INSTANCE_COUNT)
	//{
	//	particles[activeNum].active = true;
	//	activeNum++;
	//	ok = 0.0f;
	//}

	ok += delTime;
	if (ok > 5.0f)
	{
		std::random_device rd;
		std::uniform_real_distribution<float> rand(-10.0f, 10.0f);

		for (auto i = 0; i < INSTANCE_COUNT; i++) {

			randVels[i] = { rand(rd), rand(rd), rand(rd) };
		}

		ok = 0.0f;
	}

	for (auto i = 0; i < INSTANCE_COUNT; i++) {

		if (particles[i].active)
		{
			particles[i].currentLife += delTime;
			//if (particles[i].currentLife >= particles[i].maxLife)
			//{
			//	particles[i].currentLife = 0.0f;
			//	std::random_device rd;
			//	std::uniform_real_distribution<float> uniformDist(-1.0, 1.0f);
			//	std::uniform_real_distribution<float> uniformDist2(0.0f, 1.0f);
			//	instanceData[i].pos = glm::vec3(0, 0, 0);
			//	//particles[i].velocity = glm::normalize(glm::vec3(uniformDist(rd), uniformDist2(rd), uniformDist(rd)));
			//}
			auto vel = getFlowField(instanceData[i].pos);
			instanceData[i].pos += (vel * (29 * delTime));
			instanceData[i].pos += (randVels[i] * (0.5f* delTime));
		}
	}

	VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();

	VkHelper::copyMemory(device, bufferSize, instanceBuffer.memory, instanceData.data());
}

void CTPApp::createCommandBuffers() {

	cmdAndDescData.commandBuffers.resize(graphics.GetSwapChain().swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = cmdAndDescData.commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)cmdAndDescData.commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, cmdAndDescData.commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = graphics.GetRenderPass();
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = graphics.GetSwapChain().swapChainExtent;

	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	for (size_t i = 0; i < cmdAndDescData.commandBuffers.size(); i++) {

		renderPassInfo.framebuffer = graphics.GetSwapChain().swapChainFramebuffers[i];

		if (vkBeginCommandBuffer(cmdAndDescData.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBeginRenderPass(cmdAndDescData.commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		//vkCmdSetViewport(cmdAndDescData.commandBuffers[i], 0, 1, &m_Viewports[0]);

		vkCmdBindDescriptorSets(cmdAndDescData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsData.pipelineLayout, 0, 1, &particleSysDesc, 0, nullptr);
		vkCmdBindPipeline(cmdAndDescData.commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsData.graphicsPipeline);
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(cmdAndDescData.commandBuffers[i], 0, 1, &vertexBuffer, offsets);
		vkCmdBindVertexBuffers(cmdAndDescData.commandBuffers[i], 1, 1, &instanceBuffer.buffer, offsets);
		vkCmdBindIndexBuffer(cmdAndDescData.commandBuffers[i], indexBuffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(cmdAndDescData.commandBuffers[i], static_cast<uint32_t>(mesh.indices.size()), INSTANCE_COUNT, 0, 0, 0);

		vkCmdEndRenderPass(cmdAndDescData.commandBuffers[i]);

		if (vkEndCommandBuffer(cmdAndDescData.commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void CTPApp::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2> poolSizes = {
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size())),
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size()))
	};
	//poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	//poolSizes[0].descriptorCount = static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size());
	//poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	//poolSizes[1].descriptorCount = static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo = VkHelper::createDescriptorPoolInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(),
		static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size()));
	//poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	//poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	//poolInfo.pPoolSizes = poolSizes.data();
	//poolInfo.maxSets = static_cast<uint32_t>(graphics.GetSwapChain().swapChainImages.size());

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void CTPApp::createFramebuffers() {
	graphics.GetSwapChain().swapChainFramebuffers.resize(graphics.GetSwapChain().swapChainImageViews.size());

	for (size_t i = 0; i < graphics.GetSwapChain().swapChainImageViews.size(); i++) {

		std::array<VkImageView, 2> attachments = {
			graphics.GetSwapChain().swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = graphics.GetRenderPass();
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = graphics.GetSwapChain().swapChainExtent.width;
		framebufferInfo.height = graphics.GetSwapChain().swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &graphics.GetSwapChain().swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void CTPApp::createDepthResources() {

	VkFormat depthFormat = VkSetupHelper::findDepthFormat(physicalDevice);

	createImage(graphics.GetSwapChain().swapChainExtent.width, graphics.GetSwapChain().swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImage);
	AllocateImageMemory(physicalDevice, device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = VkSetupHelper::createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

bool CTPApp::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void CTPApp::createTextureImage()
{
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VkHelper::createBuffer(device, image.imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
	VkHelper::AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
	VkHelper::copyMemory(device, image.imageSize, stagingBufferMemory, image.pixels);

	image.freeImage();

	createImage(image.texWidth, image.texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, textureData.textureImage);
	AllocateImageMemory(physicalDevice, device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureData.textureImage, textureData.textureImageMemory);

	transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(stagingBuffer, textureData.textureImage, static_cast<uint32_t>(image.texWidth), static_cast<uint32_t>(image.texHeight));

	transitionImageLayout(textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	vkDestroyBuffer(device, stagingBuffer, nullptr);
	vkFreeMemory(device, stagingBufferMemory, nullptr);
}

void CTPApp::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image) {
	VkImageCreateInfo imageInfo = {};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
		throw std::runtime_error("failed to create image!");
	}
}

void CTPApp::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = VkHelper::beginSingleTimeCommands(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdAndDescData.commandPool, 1);

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (hasStencilComponent(format)) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);

	VkHelper::endSingleTimeCommands(commandBuffer, device, cmdAndDescData.commandPool, 1, graphicsQueue);
}

void CTPApp::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
	VkCommandBuffer commandBuffer = VkHelper::beginSingleTimeCommands(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdAndDescData.commandPool, 1);


	VkBufferImageCopy region = {};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);

	VkHelper::endSingleTimeCommands(commandBuffer, device, cmdAndDescData.commandPool, 1, graphicsQueue);

}

void CTPApp::createTextureImageView() {

	textureData.textureImageView = VkSetupHelper::createImageView(device, textureData.textureImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
}

void CTPApp::createTextureSampler() {

	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_FALSE;
	samplerInfo.maxAnisotropy = 1;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;

	if (vkCreateSampler(device, &samplerInfo, nullptr, &textureData.textureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

void CTPApp::createUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(graphics.GetSwapChain().swapChainImages.size());
	uniformBuffersMemory.resize(graphics.GetSwapChain().swapChainImages.size());

	for (size_t i = 0; i < graphics.GetSwapChain().swapChainImages.size(); i++) {
		VkHelper::createBufferWithoutStaging(physicalDevice, device, 
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize,
			uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void CTPApp::createBuffers()
{
	VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();

	VkHelper::createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool,
		graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		bufferSize, instanceBuffer.buffer, instanceBuffer.memory, instanceData.data());

	bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();

	VkHelper::createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool,
		graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		bufferSize, vertexBuffer, vertexBufferMemory, mesh.vertices.data());

	bufferSize = sizeof(mesh.indices[0]) * mesh.indices.size();

	VkHelper::createBufferWithStaging(physicalDevice, device, cmdAndDescData.commandPool,
		graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		bufferSize, indexBuffer, indexBufferMemory, mesh.indices.data());

	//createUniformBuffers();

	//bufferSize = sizeof(UniformBufferObject);

	//uniformBuffers.resize(graphics.GetSwapChain().swapChainImages.size());
	//uniformBuffersMemory.resize(graphics.GetSwapChain().swapChainImages.size());

	//for (size_t i = 0; i < graphics.GetSwapChain().swapChainImages.size(); i++) {
	//	VkHelper::createBufferWithoutStaging(physicalDevice, device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	//		bufferSize, uniformBuffers[i], uniformBuffersMemory[i]);
	//}
}

void CTPApp::AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VkHelper::findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void CTPApp::createSyncObjects() {

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(graphics.GetSwapChain().swapChainImages.size(), VK_NULL_HANDLE);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void CTPApp::updateUniformBuffer(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(time * glm::radians(90.0f), 1.0f, time * glm::radians(90.0f)));
	ubo.view = glm::lookAt(glm::vec3(0.0f, 3.0f, -30.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	VkHelper::copyMemory(device, sizeof(ubo), uniformBuffersMemory[currentImage], &ubo);

	//void* data;
	//vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	//memcpy(data, &ubo, sizeof(ubo));
	//vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}

void CTPApp::drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, graphics.GetSwapChain().swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}


	/* Updating obejcts*/
	/////////////////////

	updateInstanceBuffer();
	scene.Update(imageIndex);
	//updateUniformBuffer(imageIndex);
	//VkHelper::copyMemory(device, sizeof(ubo), uniformBuffersMemory[currentImage], &ubo);

	/////////////////////

	if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
		vkWaitForFences(device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
	}
	imagesInFlight[imageIndex] = inFlightFences[currentFrame];

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &cmdAndDescData.commandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	vkResetFences(device, 1, &inFlightFences[currentFrame]);

	if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { graphics.GetSwapChain().swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
		framebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}