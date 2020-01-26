#pragma once
#include "App.h"

#include "VkSetupHelper.h"
#include "VkHelper.h"
#include "VkInitializer.h"
#include "Locator.h"
#include "Timer.h"
#include "Devices.h"
#include "Keyboard.h"
#include "Mesh.h"
#include "Image.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>


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

const int INSTANCE_COUNT = 1;
const int OBJECT_COUNT = 2;
const int OBJ_COUNT = 200;

const std::string MODEL_PATH = "../../../Models/sphere.obj";
const std::string TEXTURE_PATH = "textures/texture.jpg";

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
	//VkSetup::createLogicalDevice(physicalDevice, surface, device, graphicsQueue, presentQueue);
	Locator::InitDevices(new Devices(physicalDevice, device));
	Locator::InitMeshes(new Mesh());
	Locator::GetMesh()->Load("sphere");
	Locator::InitImages(new Image());
	Locator::GetImage()->Load("texture");
	Locator::GetImage()->Load("orange");
	Locator::GetDevices()->CreateLogicalDevice(surface);
	Locator::GetDevices()->CreateQueue(graphicsQueue, Queues::GRAPHICS);
	Locator::GetDevices()->CreateQueue(presentQueue, Queues::PRESENT);
	device = Locator::GetDevices()->GetDevice();
	swapchain.Init(instance);
	swapchain.CreateSurface(window);
	swapchain.Create(window);
	//graphics.createSwapChain(physicalDevice, surface, device, window);
	//graphics.createImageViews(device);
	//graphics.createRenderPass(device, physicalDevice, swapchain.swapChainImageFormat);
	createRenderPass();
	createDescriptorSetLayout();
	Locator::GetDevices()->CreateCommandPool(commandPool);
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	obj.Init("sphere", "texture", graphicsQueue);

	objs.resize(OBJ_COUNT);
	objDescs.resize(OBJ_COUNT);
	for (size_t i = 0; i < OBJ_COUNT; i++)
	{
		if (i == 2)
		{
			objs[i].Init("sphere", "orange", graphicsQueue);
			continue;
		}
		objs[i].Init("sphere", "texture", graphicsQueue);
	}

	//scene.Init(&physicalDevice, &device, window, &graphicsQueue, &presentQueue, &graphics);
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	createSyncObjects();
	createLight();
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

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device, inFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(device, commandPool, nullptr);

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

	//graphics.createSwapChain(physicalDevice, surface, device, window);
	//graphics.createImageViews(device);
	//graphics.createRenderPass(device, physicalDevice);
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createUniformBuffers();
	//scene.CreateUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();

}

void CTPApp::cleanupSwapChain() {

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	for (auto framebuffer : swapchain.swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(device, particleSysPipe, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);

	for (auto imageView : swapchain.swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}

	vkDestroySwapchainKHR(device, swapchain.swapChain, nullptr);

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void CTPApp::createRenderPass() {

	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = swapchain.swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = VkSetupHelper::findDepthFormat(physicalDevice);
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef = {};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;

	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::vector<VkAttachmentDescription> attachments = { colorAttachment, depthAttachment };

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

void CTPApp::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2> poolSizes = {
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(swapchain.swapChainImages.size() * OBJ_COUNT)),
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(swapchain.swapChainImages.size() * OBJ_COUNT))
	};

	VkDescriptorPoolCreateInfo poolInfo = VkHelper::createDescriptorPoolInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(),
		static_cast<uint32_t>(swapchain.swapChainImages.size() * OBJ_COUNT));

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void CTPApp::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkHelper::createDescriptorLayoputBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT);

	VkDescriptorSetLayoutBinding samplerLayoutBinding = VkHelper::createDescriptorLayoputBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = VkHelper::createDescSetLayoutInfo(static_cast<uint32_t>(bindings.size()), bindings.data());

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

}

void CTPApp::createDescriptorSets() {

	std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(1);
	allocInfo.pSetLayouts = layouts.data();

	for (size_t i = 0; i < OBJ_COUNT; i++) {

		if (vkAllocateDescriptorSets(device, &allocInfo, &objDescs[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets2!");
		}

		for (size_t j = 0; j < swapchain.swapChainImages.size(); j++) {

			std::vector<VkWriteDescriptorSet> descriptorWrites = {
			VkHelper::writeDescSet(objDescs[i], VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &objs[i].GetModel().uniform[j].descriptor),
			VkHelper::writeDescSet(objDescs[i], VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &objs[i].GetTexture().descriptor)
			};

			vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
		}
	}

	//if (vkAllocateDescriptorSets(device, &allocInfo, &particleSysDesc) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to allocate descriptor sets!");
	//}

	//if (vkAllocateDescriptorSets(device, &allocInfo, &objDesc) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to allocate descriptor sets2!");
	//}

	//for (size_t i = 0; i < swapchain.swapChainImages.size(); i++) {

	//	VkDescriptorBufferInfo bufferInfo = VkInitializers::DescBufferInfo(model.uniform[i].buffer, sizeof(UniformBufferObject));

	//	VkDescriptorImageInfo imageInfo = VkInitializers::DescImageInfo(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, texture1.imageView, texture1.sampler);

	//	std::vector<VkWriteDescriptorSet> descriptorWrites = {
	//	VkHelper::writeDescSet(particleSysDesc, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &bufferInfo),
	//	VkHelper::writeDescSet(particleSysDesc, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &imageInfo)
	//	};

	//	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	//	descriptorWrites = {
	//	VkHelper::writeDescSet(objDesc, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &obj.GetModel().uniform[i].descriptor),
	//	VkHelper::writeDescSet(objDesc, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &obj.GetTexture().descriptor)
	//	};

	//	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	//}
}

void CTPApp::createGraphicsPipeline() {

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = 
		VkHelper::createShaderStageInfo("shaders/basicA/basicAVert.spv", VK_SHADER_STAGE_VERTEX_BIT, device);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = 
		VkHelper::createShaderStageInfo("shaders/basicA/basicAFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, device);

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;

	bindingDescriptions = {
		Vertex::getBindingDescription()
	};

	attributeDescriptions = {
		Vertex::getAttributeDescriptions()[0],
		Vertex::getAttributeDescriptions()[1],
		Vertex::getAttributeDescriptions()[2]
	};

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = VkHelper::createViewport(0.0f, 0.0f, (float)swapchain.swapChainExtent.width, (float)swapchain.swapChainExtent.height, 0.0f, 1.0f);

	//VkViewport viewport = VkHelper::createViewport(0, 0, swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 0.0f, 1.0f);
	VkRect2D scissor = VkHelper::createScissorRect({ 0, 0 }, swapchain.swapChainExtent);

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

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	pipelineInfo.pDepthStencilState = &depthStencil;

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &particleSysPipe) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	//if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &particleSysPipe2) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to create graphics pipeline!");
	//}
}

void CTPApp::createCommandBuffers() {

	commandBuffers.resize(swapchain.swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain.swapChainExtent;

	std::array<VkClearValue, 2> clearValues = {};
	//	clearValues[0].color = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	for (size_t i = 0; i < commandBuffers.size(); i++) {

		renderPassInfo.framebuffer = swapchain.swapChainFramebuffers[i];

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize offsets[] = { 0 };
		for (size_t j = 0; j < OBJ_COUNT; j++)
		{
			vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &objDescs[j], 0, nullptr);
			vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, particleSysPipe);

			vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &objs[j].GetModel().vertex.buffer, offsets);
			vkCmdBindIndexBuffer(commandBuffers[i], objs[j].GetModel().index.buffer, 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(objs[j].GetModel().indices.size()), 1, 0, 0, 0);
		}
		//vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &particleSysDesc, 0, nullptr);
		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, particleSysPipe);

		//vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &model.vertex.buffer, offsets);
		//vkCmdBindIndexBuffer(commandBuffers[i], model.index.buffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(model.indices.size()), 1, 0, 0, 0);


		//vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &objDesc, 0, nullptr);
		//vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, particleSysPipe2);

		//vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &obj.GetModel().vertex.buffer, offsets);
		//vkCmdBindIndexBuffer(commandBuffers[i], obj.GetModel().index.buffer, 0, VK_INDEX_TYPE_UINT32);
		//vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(obj.GetModel().indices.size()), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void CTPApp::createFramebuffers() {
	swapchain.swapChainFramebuffers.resize(swapchain.swapChainImageViews.size());

	for (size_t i = 0; i < swapchain.swapChainImageViews.size(); i++) {

		std::array<VkImageView, 2> attachments = {
			swapchain.swapChainImageViews[i],
			depthImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = swapchain.swapChainExtent.width;
		framebufferInfo.height = swapchain.swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapchain.swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void CTPApp::createDepthResources() {

	VkFormat depthFormat = VkSetupHelper::findDepthFormat(physicalDevice);

	createImage(swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, depthImage);
	AllocateImageMemory(physicalDevice, device, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = VkSetupHelper::createImageView(device, depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

}

bool CTPApp::hasStencilComponent(VkFormat format) {
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
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

void CTPApp::AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements(device, image, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = Locator::GetDevices()->findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate buffer memory!");
	}

	vkBindImageMemory(device, image, imageMemory, 0);
}

void CTPApp::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {
	VkCommandBuffer commandBuffer = Locator::GetDevices()->BeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);

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

	Locator::GetDevices()->EndSingleTimeCommands(commandBuffer, 1, graphicsQueue);
}

void CTPApp::createInstances()
{
	//instanceData.resize(INSTANCE_COUNT);
	//particles.resize(INSTANCE_COUNT);

	//std::random_device rd;
	//std::uniform_real_distribution<float> uniformDist(-1.0, 1.0f);
	//std::uniform_real_distribution<float> uniformDist2(1.0f, 1.0f);
	//std::uniform_real_distribution<float> randStartPos(-10.0f, 10.0f);

	//for (auto i = 0; i < INSTANCE_COUNT; i++) {
	//	instanceData[i].pos = glm::vec3(randStartPos(rd), randStartPos(rd), 0);
	//	//instanceData[i].pos = {0, 1, 0};
	//	instanceData[i].rot = glm::vec3(uniformDist(rd), uniformDist(rd), 0);
	//	instanceData[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//	instanceData[i].scale = 1.0f;
	//	instanceData[i].texIndex = 0;
	//	particles[i].maxLife = 1.0f;
	//	particles[i].currentLife = 0.0f;
	//	particles[i].velocity = glm::vec3(0, 0, 0);
	//	particles[i].active = true;
	//	particles[i].speed = uniformDist2(rd);
	//	for (auto j = 0; j < mesh.vertices.size(); j++) {
	//		if (j == 0)
	//		{
	//			minDist = glm::distance(instanceData[i].pos + mesh.vertices[j].pos, light.pos);
	//		}
	//		else
	//		{
	//			auto newDist = glm::distance(instanceData[i].pos + mesh.vertices[j].pos, light.pos);
	//			if (newDist < minDist)
	//			{
	//				minDist = newDist;
	//			}
	//		}
	//	}
	//}

	//std::random_device rd;
	//std::uniform_real_distribution<float> uniformDist(-1.0, 1.0f);
	//std::uniform_real_distribution<float> uniformDist2(1.0f, 3.0f);
	//std::uniform_real_distribution<float> randStartPos(-10.0f, 10.0f);
	//particles.resize(OBJECT_COUNT);
	//for (auto i = 0; i < OBJECT_COUNT; i++) {
	//	model[i].pos = glm::vec3(randStartPos(rd), randStartPos(rd), 0);
	//	model[i].rot = glm::vec3(uniformDist(rd), uniformDist(rd), 0);
	//	model[i].color = { 1.0f, 1.0f, 1.0f, 1.0f };
	//	model[i].scale = 1.0f;
	//	particles[i].maxLife = 1.0f;
	//	particles[i].currentLife = 0.0f;
	//	particles[i].velocity = glm::vec3(0, 0, 0);
	//	particles[i].active = true;
	//	particles[i].speed = uniformDist2(rd);

	//	minDists.push_back(0.0f);

	//	for (auto j = 0; j < meshes[i].vertices.size(); j++) {
	//		if (j == 0)
	//		{
	//			minDists[i] = glm::distance(model[i].pos + meshes[i].vertices[j].pos, light.pos);
	//		}
	//		else
	//		{
	//			auto newDist = glm::distance(model[i].pos + meshes[i].vertices[j].pos, light.pos);
	//			if (newDist < minDists[i])
	//			{
	//				minDists[i] = newDist;
	//			}
	//		}
	//	}
	//}

}

glm::vec3 CTPApp::getFlowField(glm::vec3 pos)
{
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, -pos.x * pos.y) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z)));
	glm::vec3 vel = (glm::vec3(-pos.y, pos.x, 0) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y)));
	//vel.x = std::sqrt((pos.x * pos.x) + (pos.y * pos.y));
	//vel.y = 0;
	//vel.z = 0;

	return vel;
}

void CTPApp::updateInstanceBuffer() {

	//static auto startTime = std::chrono::high_resolution_clock::now();

	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
	//auto delTime = Locator::GetTimer()->DeltaTime();

	////	//ok += delTime;
	////	//if (ok > 0.1f && activeNum < INSTANCE_COUNT)
	////	//if (ok > 1.0f)
	////	//{
	////	//	//particles[activeNum].active = true;
	////	//	//activeNum++;
	////	//	std::random_device rd;
	////	//	std::uniform_real_distribution<float> uniformDist(0.0f, 1.0f);
	////	//	for (auto i = 0; i < INSTANCE_COUNT; i++) {
	////	//		instanceData[i].color = { uniformDist(rd) ,uniformDist(rd) , uniformDist(rd) , 1.0f };
	////	//	//	instanceData[i].color = { 0.0f, 0.0f, 0.0f , 1.0f };
	////	//	}
	////	//	ok = 0.0f;
	////	//}
	////
	////	for (auto i = 0; i < INSTANCE_COUNT; i++) {
	////		//if (checkDistanceFromLight(instanceData[i].pos))
	////		//{
	////			for (auto j = 0; j < mesh.vertices.size(); j++) {
	////
	////				float dist[2] = { glm::distance(instanceData[i].pos + mesh.vertices[j].pos, light.pos), glm::distance(instanceData[i].pos, light.pos) };
	////				if (dist[0] < dist[1])
	////				{
	////					float curDist = dist[0] - minDist;
	////					float maxDist = dist[1] - minDist;
	////					auto amount = curDist / maxDist;
	////					mesh.vertices[j].color = { 1.0f - amount, 1.0f - amount, 0.0f, 1.0f };
	////				}
	////				else
	////					mesh.vertices[j].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	////			}
	////		//}
	////		//else
	////		//	instanceData[i].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	////
	////		particles[i].currentLife += delTime;
	////		auto vel = getFlowField(instanceData[i].pos);
	////	
	////		instanceData[i].pos += (vel * (1 * delTime) * particles[i].speed);
	////
	////		//if (particles[i].currentLife >= particles[i].maxLife)
	////		//{
	////		//	particles[i].currentLife = 0.0f;
	////		//	std::random_device rd;
	////		//	std::uniform_real_distribution<float> uniformDist(-1.0, 1.0f);
	////		//	std::uniform_real_distribution<float> uniformDist2(0.0f, 1.0f);
	////		//	instanceData[i].pos = glm::vec3(0, 0, 0);
	////		//	//particles[i].velocity = glm::normalize(glm::vec3(uniformDist(rd), uniformDist2(rd), uniformDist(rd)));
	////		//}
	////	}
	////
	////	remapInstanceData();

	//for (auto i = 0; i < OBJECT_COUNT; i++) {
	//	for (auto j = 0; j < meshes[i].vertices.size(); j++) {

	//		float dist[2] = { glm::distance(model[i].pos + meshes[i].vertices[j].pos, light.pos), glm::distance(model[i].pos, light.pos) };

	//		if (dist[0] < dist[1])
	//		{
	//			float curDist = dist[0] - minDists[i];
	//			float maxDist = dist[1] - minDists[i];
	//			auto amount = curDist / maxDist;

	//			meshes[i].vertices[j].color = { 1.0f - amount, 1.0f - amount, 0.0f, 1.0f };
	//		}
	//		else
	//			meshes[i].vertices[j].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	//	}

	//	auto vel = getFlowField(model[i].pos);

	//	model[i].pos += (vel * (1 * delTime) * particles[i].speed);
	//}
	//remapVertexData();
}

void CTPApp::createUniformBuffers() {

	for (size_t i = 0; i < OBJ_COUNT; i++) {

		objs[i].GetModel().uniform.resize(swapchain.swapChainImages.size());

		for (size_t j = 0; j < swapchain.swapChainImages.size(); j++) {

			objs[i].GetModel().uniform[j].CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));

			objs[i].GetModel().uniform[j].UpdateDescriptor(sizeof(UniformBufferObject));
		}
		objs[i].GetModel().transform.pos = { i / (i * 0.5), i, 0 };
	}



	//model.uniform.resize(swapchain.swapChainImages.size());
	//obj.GetModel().uniform.resize(swapchain.swapChainImages.size());

	//for (size_t j = 0; j < swapchain.swapChainImages.size(); j++) {

	//	model.uniform[j].CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));

	//	obj.GetModel().uniform[j].CreateBuffer(device, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	//		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));

	//	model.uniform[j].UpdateDescriptor(sizeof(UniformBufferObject));

	//	obj.GetModel().uniform[j].UpdateDescriptor(sizeof(UniformBufferObject));
	//}

	//model.transform.pos = { 0, 12, 0 };
	//obj.GetModel().transform.pos = { 0, 3, 0 };
}

void CTPApp::updateUniformBuffer(uint32_t currentImage) {

	//model.transform.pos += getFlowField(model.transform.pos) * Locator::GetTimer()->DeltaTime() * 5.f;
	//obj.GetModel().transform.pos += getFlowField(obj.GetModel().transform.pos) * Locator::GetTimer()->DeltaTime() * 3.f;

	//static auto startTime = std::chrono::high_resolution_clock::now();

	//auto currentTime = std::chrono::high_resolution_clock::now();
	//float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	//UniformBufferObject ubo = {};
	////	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(5, 5, 5));
	//ubo.model = glm::translate(glm::mat4(1.0f), model.transform.pos) * glm::scale(glm::mat4(1.0f), model.transform.scale);
	//ubo.view = glm::lookAt(camPos, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
	//ubo.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	//ubo.proj[1][1] *= -1;

	//model.uniform[currentImage].CopyMem(&ubo, sizeof(ubo));

	//ubo.model = glm::translate(glm::mat4(1.0f), obj.GetModel().transform.pos) * glm::scale(glm::mat4(1.0f), obj.GetModel().transform.scale);
	//ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//ubo.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	//ubo.proj[1][1] *= -1;
	//obj.GetModel().uniform[currentImage].CopyMem(&ubo, sizeof(ubo));

	for (size_t i = 0; i < OBJ_COUNT; i++) {

		objs[i].GetModel().transform.pos += getFlowField(objs[i].GetModel().transform.pos) * Locator::GetTimer()->DeltaTime() * 30.f;

		static auto startTime = std::chrono::high_resolution_clock::now();

		auto currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

		UniformBufferObject ubo = {};
		ubo.model = glm::translate(glm::mat4(1.0f), objs[i].GetModel().transform.pos) * glm::scale(glm::mat4(1.0f), objs[i].GetModel().transform.scale);
		ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		ubo.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
		ubo.proj[1][1] *= -1;

		objs[i].GetModel().uniform[currentImage].CopyMem(&ubo, sizeof(ubo));
	}

}

void CTPApp::createLight()
{
	light.pos = { 0, 0, 0 };
	light.radius = 20;
}

bool CTPApp::checkDistanceFromLight(glm::vec3 pos)
{
	return glm::distance(pos, light.pos) < light.radius;
}

void CTPApp::remapInstanceData()
{
	VkDeviceSize bufferSize = sizeof(InstanceData) * instanceData.size();

	//VkHelper::copyMemory(device, bufferSize, instanceBuffer.memory, instanceData.data());
	//VkHelper::createBufferWithStaging(physicalDevice, device, commandPool,
	//	graphicsQueue, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	//	bufferSize, instanceBuffer.buffer, instanceBuffer.memory, instanceData.data());
}

void CTPApp::remapVertexData()
{

	//VkDeviceSize bufferSize = sizeof(mesh.vertices[0]) * mesh.vertices.size();

	//VkHelper::copyMemory(device, bufferSize, vertexBufferMemory, mesh.vertices.data());

	//VkDeviceSize bufferSize = sizeof(Model) * OBJECT_COUNT;

	//VkHelper::copyMemory(device, bufferSize, modelBuffer.memory, model.data());

	//for (size_t i = 0; i < OBJECT_COUNT; i++)
	//{
	//	VkDeviceSize bufferSize = sizeof(Model);

	//	VkHelper::copyMemory(device, bufferSize, modelBuffers[i].memory, model.data());

	//	bufferSize = sizeof(meshes[i].vertices[0]) * meshes[i].vertices.size();

	//	VkHelper::copyMemory(device, bufferSize, meshes[i].vertexBufferMemory, meshes[i].vertices.data());

	//}
}

void CTPApp::createSyncObjects() {

	imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
	imagesInFlight.resize(swapchain.swapChainImages.size(), VK_NULL_HANDLE);

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

void CTPApp::drawFrame() {
	vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(device, swapchain.swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		recreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error("failed to acquire swap chain image!");
	}


	/* Updating obejcts*/
	/////////////////////

	Update();
	//updateInstanceBuffer();
	//scene.Update(imageIndex);
	updateUniformBuffer(imageIndex);
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
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];

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

	VkSwapchainKHR swapChains[] = { swapchain.swapChain };
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

void CTPApp::Update()
{

	camPos = glm::vec3(sin(angleX) * distFromOrigin, sin(angleY) * distFromOrigin, cos(angleX) * distFromOrigin);

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_4))
	{
		angleX -= angleSpeed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_6))
	{
		angleX += angleSpeed * Locator::GetTimer()->DeltaTime();
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_8) && angleY < 1.5f)
	{
		angleY += angleSpeed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_5) && angleY > -1.5f)
	{
		angleY -= angleSpeed * Locator::GetTimer()->DeltaTime() * 0.5f;
	}

	//camPos *= distFromOrigin;


	auto diff = camPos - glm::vec3(0, 0, 0);

	diff = glm::normalize(diff);

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_7))
	{
		distFromOrigin += camSpeed * Locator::GetTimer()->DeltaTime();
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_KP_9))
	{
		distFromOrigin -= camSpeed * Locator::GetTimer()->DeltaTime();
	}

	camPos = diff * distFromOrigin;
}