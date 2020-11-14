#include "Scene.h"

#include "VkSetupHelper.h"
#include "VkHelper.h"
#include "VkInitializer.h"
#include "Locator.h"
#include "Timer.h"
#include "Devices.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Mesh.h"
#include "Image.h"
#include "Shaders.h"
#include "Constants.h"
#include "ThreadManager.h"
#include "DebugPrinter.h"
#include "Utillities.h"
#include "Output.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/vec3.hpp>


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
#include <random>
#include <thread>
#include <mutex>
#include <limits>

using namespace std::chrono_literals;

#include <algorithm>


Output output;

Scene::~Scene()
{

}

void Scene::run() {

	initWindow();
	initVulkan();
	LocatorSetup();
	createDescriptorSetLayout();
	createPipelineLayout();
	createGraphicsPipeline();
	LoadAssets();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCompute();
	createCommandBuffers();
	mainLoop();
	Cleanup();
}

void Scene::RecreateSwapChain() {
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(device);

	//vkDestroyPipeline(device, objectPipeline, nullptr);
	//vkDestroyPipeline(device, pSystemPipeline, nullptr);
	CleanUpObjects();
	cleanupSwapChain();
	cleanupCompute();

	vkDestroySurfaceKHR(swapchain.instance, swapchain.surface, nullptr);

	swapchain.CreateSurface(window);
	swapchain.Create(window);
	createRenderPass();
	createPipelineLayout();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	LoadAssets();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCompute();
	createCommandBuffers();
}

void Scene::LocatorSetup()
{
	Locator::InitMeshes(new Mesh());
	Locator::GetMesh()->LoadAll();

	Locator::InitImages(new Image());
	Locator::GetImage()->Load("texture");
	Locator::GetImage()->Load("particle");
	Locator::GetImage()->Load("particle2");
	Locator::GetImage()->Load("spaceBackground3");
	Locator::GetImage()->Load("spaceSkybox2");
	Locator::GetImage()->Load("blank");

	Locator::InitShader(new Shaders());
	Locator::InitThreadManager(new ThreadManager());
	Locator::GetDevices()->CreateCommandPool(commandPool, Queues::GRAPHICS);

}

void Scene::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Locator::GetTimer()->GetTimePoint((float)glfwGetTime());
		drawFrame();
	}
	vkDeviceWaitIdle(device);
}

void Scene::createDescriptorPool() {

	std::vector<VkDescriptorPoolSize> poolSizes = {
		VkInitializer::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 8),
		VkInitializer::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2),
		VkInitializer::DescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 6),
	};

	VkDescriptorPoolCreateInfo poolInfo = VkInitializer::DescriptorPoolInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(),
		8);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Scene::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkInitializer::DescriptorLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);

	VkDescriptorSetLayoutBinding samplerLayoutBinding = VkInitializer::DescriptorLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	VkDescriptorSetLayoutBinding lightLayoutBinding = VkInitializer::DescriptorLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	VkDescriptorSetLayoutBinding lightUboLayoutBinding = VkInitializer::DescriptorLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

	//VkDescriptorSetLayoutBinding texture3DLayoutBinding = VkInitializer::DescriptorLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding, lightLayoutBinding, lightUboLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = VkInitializer::DescSetLayoutInfo(static_cast<uint32_t>(bindings.size()), bindings.data());

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}

}

void Scene::createPipelineLayout()
{
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}
}

void Scene::createDescriptorSets() {
	
	std::vector<VkDescriptorSetLayout> layouts(1, descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(device, &allocInfo, &pSystemDescSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites = {
	VkInitializer::WriteDescSet(pSystemDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &particle_system.UBuffer().descriptor),
	VkInitializer::WriteDescSet(pSystemDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &particle_system.PsTexture().descriptor)
	};

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	if (vkAllocateDescriptorSets(device, &allocInfo, &objectDescSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	descriptorWrites = {
	VkInitializer::WriteDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &object.GetModel().uniform.descriptor),
	VkInitializer::WriteDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &object.GetTexture().descriptor),
	VkInitializer::WriteDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, &particle_system.PBuffer().descriptor),
	VkInitializer::WriteDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, &lightUboBuffer.descriptor),
	//VkInitializer::WriteDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4, &texture3Ddescriptor),
	};

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
}

void Scene::createGraphicsPipeline() {

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = VkInitializer::Viewport(0.0f, 0.0f, (float)swapchain.extent.width, (float)swapchain.extent.height, 0.0f, 1.0f);

	//VkViewport viewport = VkInitializer::createViewport(0, 0, swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 0.0f, 1.0f);
	VkRect2D scissor = VkInitializer::ScissorRect({ 0, 0 }, swapchain.extent);

	VkPipelineViewportStateCreateInfo viewportState = VkInitializer::ViewportStateInfo(1, 1, &viewport, &scissor);

	VkPipelineRasterizationStateCreateInfo rasterizer = VkInitializer::Rasteriser(
		VK_FALSE, VK_FALSE,
		VK_POLYGON_MODE_FILL, 1.0f,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_TRUE);


	VkPipelineMultisampleStateCreateInfo multisampling =
		VkInitializer::MultiSampling(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

	VkPipelineColorBlendAttachmentState colorBlendAttachment =
		VkInitializer::ColourBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);

	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = VkInitializer::ColourBlendStateInfo(
		VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment, { 0.0f, 0.0f, 0.0f, 0.0f });

	VkPipelineDepthStencilStateCreateInfo depthStencil = VkInitializer::DepthStencilInfo(
		VK_TRUE, VK_TRUE,
		VK_COMPARE_OP_LESS,
		VK_FALSE, VK_FALSE);

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
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
	
	std::vector<VkVertexInputBindingDescription> bindingDescriptions = {
		Vertex::getBindingDescription(),
		//VkHelper::createVertexBindingDescription(1, sizeof(int), VK_VERTEX_INPUT_RATE_VERTEX),
		//Light::getBindingDescription()
	};

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		Vertex::getAttributeDescriptions(),
	};

	//attributeDescriptions.emplace_back(VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32_SINT, sizeof(int)));
	//attributeDescriptions.emplace_back(Light::getAttributeDescriptions()[0]);
	//attributeDescriptions.emplace_back(Light::getAttributeDescriptions()[1]);
	//attributeDescriptions.emplace_back(Light::getAttributeDescriptions()[2]);

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = Locator::GetShader()->CreateShaderInfo("ggxShaderVert", VK_SHADER_STAGE_VERTEX_BIT);
		//VkHelper::createShaderStageInfo("shaders/basicA/basicAVert.spv", VK_SHADER_STAGE_VERTEX_BIT, device);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = Locator::GetShader()->CreateShaderInfo("ggxShaderFrag", VK_SHADER_STAGE_FRAGMENT_BIT);
	//VkHelper::createShaderStageInfo("shaders/basicA/basicAFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, device);

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

	shaderStages = { vertShaderStageInfo, fragShaderStageInfo };

	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &objectPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;

	depthStencil.depthWriteEnable = VK_FALSE;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;


	bindingDescriptions = { Particle::getBindingDescription() };
	//bindingDescriptions.push_back(VkHelper::createVertexBindingDescription(1, sizeof(float), VK_VERTEX_INPUT_RATE_VERTEX));

	attributeDescriptions = { Particle::getAttributeDescriptions() };

	//attributeDescriptions.push_back(VkHelper::createVertexAttributeDescription(1, 3, VK_FORMAT_R32_SFLOAT, sizeof(float)));

	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vertShaderStageInfo = Locator::GetShader()->CreateShaderInfo("pointSpriteVert", VK_SHADER_STAGE_VERTEX_BIT);
		//VkHelper::createShaderStageInfo("shaders/pointSprite/pointSpriteVert.spv", VK_SHADER_STAGE_VERTEX_BIT, device);

	fragShaderStageInfo = Locator::GetShader()->CreateShaderInfo("pointSpriteFrag", VK_SHADER_STAGE_FRAGMENT_BIT);
		//VkHelper::createShaderStageInfo("shaders/pointSprite/pointSpriteFrag.spv", VK_SHADER_STAGE_FRAGMENT_BIT, device);

	VkPipelineShaderStageCreateInfo geomShaderStageInfo = Locator::GetShader()->CreateShaderInfo("pointSpriteGeom", VK_SHADER_STAGE_GEOMETRY_BIT);
		//VkHelper::createShaderStageInfo("shaders/pointSprite/pointSpriteGeom.spv", VK_SHADER_STAGE_GEOMETRY_BIT, device);

	shaderStages = { vertShaderStageInfo, fragShaderStageInfo, geomShaderStageInfo };

	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pSystemPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	Locator::GetShader()->DestroyShaders();
}

void Scene::createCommandBuffers() {

	commandBuffers.resize(swapchain.framebuffers.size());

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
	//beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain.extent;

	std::array<VkClearValue, 2> clearValues = {};
	//clearValues[0].color = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
	clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	for (size_t i = 0; i < commandBuffers.size(); i++) {

		renderPassInfo.framebuffer = swapchain.framebuffers[i];

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkBufferMemoryBarrier buffer_barrier = {};
		
		buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		buffer_barrier.srcAccessMask = 0;
		buffer_barrier.dstAccessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
		buffer_barrier.srcQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value();
		buffer_barrier.dstQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value();
		buffer_barrier.buffer = particle_system.PBuffer().buffer;
		buffer_barrier.size = particle_system.PBuffer().size;

		vkCmdPipelineBarrier(
			commandBuffers[i],
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			1, &buffer_barrier,
			0, nullptr);

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize offsets[] = { 0 };

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &objectDescSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &object.GetModel().vertex.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], object.GetModel().index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(object.GetModel().indices.size()), 1, 0, 0, 0);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &pSystemDescSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pSystemPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &particle_system.PBuffer().buffer, offsets);
		vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(particle_system.ParticleCount()), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		buffer_barrier.dstAccessMask = 0;
		buffer_barrier.srcQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value();
		buffer_barrier.dstQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value();
		buffer_barrier.buffer = particle_system.PBuffer().buffer;
		buffer_barrier.size = particle_system.PBuffer().size;

		vkCmdPipelineBarrier(
			commandBuffers[i],
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			0, nullptr,
			1, &buffer_barrier,
			0, nullptr);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

// Find and create a compute capable device queue
void Scene::GetComputeQueue()
{
	uint32_t queueFamilyCount;
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, NULL);
	assert(queueFamilyCount >= 1);

	std::vector<VkQueueFamilyProperties> queueFamilyProperties;
	queueFamilyProperties.resize(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

	// Some devices have dedicated compute queues, so we first try to find a queue that supports compute and not graphics
	bool computeQueueFound = false;
	for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
	{
		if ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
		{
			compute.queueFamilyIndex = i;
			computeQueueFound = true;
			break;
		}
	}
	// If there is no dedicated compute queue, just find the first queue family that supports compute
	if (!computeQueueFound)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
		{
			if (queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				compute.queueFamilyIndex = i;
				computeQueueFound = true;
				break;
			}
		}
	}

	// Compute is mandatory in Vulkan, so there must be at least one queue family that supports compute
	assert(computeQueueFound);
	// Get a compute queue from the device
	vkGetDeviceQueue(device, Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value(), 0, &compute.queue);
}

void Scene::createCompute() {

	GetComputeQueue();

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value();
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	if (vkCreateCommandPool(device, &poolInfo, nullptr, &compute.commandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute command pool!");
	}

	ffmodel_buffer.CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
		sizeof(Triangle) * 500000);
	//ffmodel_buffer.StageBuffer(ffmodel_buffer.size, compute.queue, ffModel.triangles.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, compute.commandPool);
	//ffmodel_buffer.Map(sizeof(Triangle) * 500000, 0);
	ffmodel_buffer.UpdateDescriptor(sizeof(Triangle) * 500000);

	TriangleUBO triUBO{};

	triUBO.triangle_count = ffModel.triangles.size();
	triUBO.vertex_per_triangle = 3;
	triUBO.max = ffModel.max;
	triUBO.min = ffModel.min;

	triangle_ubo_buffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(TriangleUBO));

	triangle_ubo_buffer.CopyMem(&triUBO, sizeof(triUBO));

	triangle_ubo_buffer.UpdateDescriptor(sizeof(triUBO));

	//particleLightBuffer.CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Light) * lights.size());
	//particleLightBuffer.StageBuffer(particleLightBuffer.size, compute.queue, lights.data(), particleLightBuffer.memProperties, compute.commandPool);

	//particleLightBuffer.UpdateDescriptor(sizeof(Light) * lights.size());

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkInitializer::DescriptorLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	VkDescriptorSetLayoutBinding storageLayoutBinding = VkInitializer::DescriptorLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	VkDescriptorSetLayoutBinding ubo2LayoutBinding = VkInitializer::DescriptorLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	VkDescriptorSetLayoutBinding storage2LayoutBinding = VkInitializer::DescriptorLayoutBinding(3, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	//VkDescriptorSetLayoutBinding storage3LayoutBinding = VkInitializer::DescriptorLayoutBinding(4, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_COMPUTE_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, storageLayoutBinding,  ubo2LayoutBinding, storage2LayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = VkInitializer::DescSetLayoutInfo(static_cast<uint32_t>(bindings.size()), bindings.data());

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &compute.descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute descriptor set layout!");
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &compute.descriptorSetLayout;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &compute.pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline layout!");
	}

	std::vector<VkDescriptorSetLayout> layouts(1, compute.descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = layouts.data();

	if (vkAllocateDescriptorSets(device, &allocInfo, &compute.descriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	std::vector<VkWriteDescriptorSet> descriptorWrites = {
		VkInitializer::WriteDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &particle_ubo_buffer.descriptor),
		VkInitializer::WriteDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, &particle_system.PBuffer().descriptor),
		VkInitializer::WriteDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &triangle_ubo_buffer.descriptor),
		VkInitializer::WriteDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3, &ffmodel_buffer.descriptor),
		//VkInitializer::WriteDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 4, &texture3Ddescriptor)
	};

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.layout = compute.pipelineLayout;
	computePipelineCreateInfo.flags = 0;

	computePipelineCreateInfo.stage = Locator::GetShader()->CreateShaderInfo("pointSpriteComp", VK_SHADER_STAGE_COMPUTE_BIT);
	if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &compute.pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}



	//VkCommandBufferAllocateInfo cmdAllocInfo = {};
	//cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	//cmdAllocInfo.commandPool = compute.commandPool;
	//cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	//cmdAllocInfo.commandBufferCount = 1;

	//if (vkAllocateCommandBuffers(device, &cmdAllocInfo, &compute.commandBuffer) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to allocate compute command buffers!");
	//}

	//// Fence for compute CB sync
	//VkFenceCreateInfo fenceCreateInfo{};
	//fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	//if (vkCreateFence(device, &fenceCreateInfo, nullptr, &compute.fence) != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to create compute fences!");
	//}

	compute.commandBuffers.resize(3);
	compute.fences.resize(2);

	for (size_t i = 0; i < 3; i++)
	{
		VkCommandBufferAllocateInfo cmdAllocInfo = {};
		cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cmdAllocInfo.commandPool = compute.commandPool;
		cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cmdAllocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(device, &cmdAllocInfo, &compute.commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate compute command buffers!");
		}

		if (i > 1)
			continue;

		// Fence for compute CB sync
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		if (vkCreateFence(device, &fenceCreateInfo, nullptr, &compute.fences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create compute fences!");
		}

	}

	//VkSemaphoreCreateInfo computeSemaphoreCreateInfo{};
	//computeSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	//if (vkCreateSemaphore(device, &computeSemaphoreCreateInfo, nullptr, &compute.semaphore) != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to create compute semaphore!");
	//}
	//VkSemaphoreCreateInfo graphicsSemaphoreCreateInfo{};
	//graphicsSemaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	//if (vkCreateSemaphore(device, &graphicsSemaphoreCreateInfo, nullptr, &graphicsSemaphore) != VK_SUCCESS)
	//{
	//	throw std::runtime_error("failed to create compute semaphore!");
	//}
	//VkSubmitInfo submitInfo{};
	//submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//submitInfo.signalSemaphoreCount = 1;
	//submitInfo.pSignalSemaphores = &compute.semaphore;
	//if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, NULL) != VK_SUCCESS) {
	//	throw std::runtime_error("failed to sync compute semaphore!");
	//}
	//vkQueueWaitIdle(graphicsQueue);


	BuildComputeCommandBuffer();
	
	// If graphics and compute queue family indices differ, acquire and immediately release the storage buffer, so that the initial acquire from the graphics command buffers are matched up properly
	if (Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value() !=
		Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value())
	{
		// Create a transient command buffer for setting up the initial buffer transfer state
		VkCommandBuffer transferCmd = Locator::GetDevices()->BeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1,  compute.commandPool);
		VkBufferMemoryBarrier acquire_buffer_barrier =
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_SHADER_READ_BIT,
			VK_ACCESS_SHADER_WRITE_BIT,
			Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value(),
			Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value(),
			particle_system.PBuffer().buffer,
			0,
			particle_system.PBuffer().size
		};
		vkCmdPipelineBarrier(transferCmd, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0, 0, nullptr, 1, &acquire_buffer_barrier, 0, nullptr);

		VkBufferMemoryBarrier release_buffer_barrier =
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_SHADER_WRITE_BIT,
			0,
			Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value(),
			Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value(),
			particle_system.PBuffer().buffer,
			0,
			particle_system.PBuffer().size
		};
		vkCmdPipelineBarrier(
			transferCmd, VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0, 0, nullptr, 1, &release_buffer_barrier, 0, nullptr);

		Locator::GetDevices()->EndSingleTimeCommands(transferCmd, 1, compute.commandPool, compute.queue);
	}
	Locator::GetShader()->DestroyShaders();
}

void Scene::BuildComputeCommandBuffer()
{
	//vkQueueWaitIdle(compute.queue);

	for (size_t i = 0; i < 3; i++)
	{
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(compute.commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkBufferMemoryBarrier buffer_barrier{};

		buffer_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		buffer_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		buffer_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		buffer_barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_barrier.srcQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value();
		buffer_barrier.dstQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value();
		buffer_barrier.buffer = particle_system.PBuffer().buffer;
		buffer_barrier.size = particle_system.PBuffer().size;

		vkCmdPipelineBarrier(
			compute.commandBuffers[i],
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			0, nullptr,
			1, &buffer_barrier,
			0, nullptr);

		vkCmdBindPipeline(compute.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipeline);
		vkCmdBindDescriptorSets(compute.commandBuffers[i], VK_PIPELINE_BIND_POINT_COMPUTE, compute.pipelineLayout, 0, 1, &compute.descriptorSet, 0, 0);
		vkCmdDispatch(compute.commandBuffers[i], particle_system.ParticleCount() / COMPUTE_PROCESS_NUM, 1, 1);

		buffer_barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
		buffer_barrier.dstAccessMask = 0;
		buffer_barrier.srcQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value();
		buffer_barrier.dstQueueFamilyIndex = Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value();
		buffer_barrier.buffer = particle_system.PBuffer().buffer;
		buffer_barrier.size = particle_system.PBuffer().size;

		vkCmdPipelineBarrier(
			compute.commandBuffers[i],
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			0,
			0, nullptr,
			1, &buffer_barrier,
			0, nullptr);

		vkEndCommandBuffer(compute.commandBuffers[i]);
	}
}

void Scene::createUniformBuffers() 
{
	lights.resize(particle_system.ParticleCount());

	for (size_t i = 0; i < particle_system.ParticleCount(); i++)
	{
		//lights[i].col = glm::vec3(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f);
		lights[i].col = glm::vec3(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
		lights[i].pos = particle_system.Particles(i).position;
	}

	//lightBuffer.CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Light) * lights.size());
	//lightBuffer.StageBuffer(lightBuffer.size, graphicsQueue, lights.data(), lightBuffer.memProperties);

	//lightBuffer.UpdateDescriptor(sizeof(Light) * lights.size());

	uboLight.camPos = camera.GetTransform().position;
	//uboLight.lightCount = particle_system.ParticleCount();

	//lgh.Create(lights);

	//lightBuffer.CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Light) * lgh.Lights().size());
	//lightBuffer.StageBuffer(lightBuffer.size, graphicsQueue, lgh.Lights().data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	//lightBuffer.UpdateDescriptor(sizeof(Light) * lgh.Lights().size());

	particle_ubo_buffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(ParticleUBO));

	particle_ubo_buffer.UpdateDescriptor(sizeof(ParticleUBO));

	ParticleUBO particle_ubo;
	particle_ubo.delta_time = Locator::GetTimer()->DeltaTime();
	particle_ubo.particle_count = particle_system.ParticleCount();
	particle_ubo.randomVec = glm::vec2(Utillities::GetRandomFloat(0.0f, 1.0f), Utillities::GetRandomFloat(0.0f, 1.0f));
	particle_ubo.resolution = glm::vec2(WIDTH, HEIGHT);
	particle_ubo_buffer.CopyMem(&particle_ubo, sizeof(particle_ubo));


	lightUboBuffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(LightUBO));

	lightUboBuffer.UpdateDescriptor(sizeof(LightUBO));

	object.GetModel().uniform.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBuffer));

	object.GetModel().uniform.UpdateDescriptor(sizeof(UniformBuffer));
}

void Scene::updateUniformBuffer(uint32_t currentImage) {

	UniformBuffer ubo = {};

	//object.GetModel().transform.pos += getFlowField(object.GetModel().transform.pos) * Locator::GetTimer()->DeltaTime();

	//ubo.model = glm::scale(glm::mat4(1.0f), object.GetTransform().scale) *
	//	glm::rotate(glm::mat4(1.0f), rotTime, glm::vec3(12, 12, 1)) *

	ubo.model = glm::translate(glm::mat4(1.0f), object.GetTransform().position);
	ubo.model = glm::scale(ubo.model, object.GetTransform().scale);
	ubo.model = glm::rotate(ubo.model, glm::radians(-110.0f), glm::vec3(0, 0, 1));
	ubo.view = camera.ViewMatrix();
	ubo.proj = perspective;

	object.GetModel().uniform.CopyMem(&ubo, sizeof(ubo));


	std::random_device rd;
	std::uniform_real_distribution<float> rand(0.0f, 1.0f);

	particle_ubo.delta_time = Locator::GetTimer()->DeltaTime();
	particle_ubo.particle_count = particle_system.ParticleCount();
	particle_ubo.randomVec = glm::vec2(Utillities::GetRandomFloat(0.0f, 1.0f), Utillities::GetRandomFloat(0.0f, 1.0f));
	particle_ubo.resolution = glm::vec2(WIDTH, HEIGHT);
	particle_ubo.flow = flow;
	particle_ubo_buffer.CopyMem(&particle_ubo, sizeof(particle_ubo));
}

void Scene::LoadAssets()
{
	if(!Locator::GetThreadManager()->IsInitialised())
		Locator::GetThreadManager()->Init(20, true);

	perspective = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	perspective[1][1] *= -1;

	camera.Setup(glm::vec3(0.0f, 0.0f, 10.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);

	particle_system.Create(graphicsQueue, &camera.ViewMatrix(), &perspective);

	//nearestTri.resize(particle_system.ParticleCount());

	object.Init("square", "blank", graphicsQueue);

	Transform transform;
	transform.position = { -210.0f, 180.0f, 0.0f };
	transform.scale = glm::vec3(100.0f);
	//transform.scale = { 4.0f, 4.0f, 4.0f };

	object.SetTransform(transform);

	Transform trans;
	trans.position = { 0.0f, 0.0f, 0.0f };
	trans.scale = glm::vec3(20.0f);
	ffModel.Load("dog", trans);

	//kdTree.Init(ffModel.triangles);

	//UpdateParticleBehaviour();
}

void Scene::Update()
{
	//if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_SPACE))
	//{
	//	output.Outputy();
	//}
	//else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_H))
	//{
	//}
	//else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_J))
	//{
	//}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_J))
	{
		Transform trans;
		trans.position = { 0.0f, 0.0f, 0.0f };
		trans.scale = glm::vec3(30.0f);
		ffModel.Load("cat", trans);

		//ffmodel_buffer.UpdateDescriptor(sizeof(Triangle) * ffModel.triangles.size());
		ffmodel_buffer.CopyMem(ffModel.triangles.data(), sizeof(Triangle) * ffModel.triangles.size());

		TriangleUBO triUBO{};

		triUBO.triangle_count = ffModel.triangles.size();
		triUBO.vertex_per_triangle = 3;
		triUBO.max = ffModel.max;
		triUBO.min = ffModel.min;

		triangle_ubo_buffer.CopyMem(&triUBO, sizeof(triUBO));
	}
	else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_H))
	{
		Transform trans;
		trans.position = { 0.0f, 0.0f, 0.0f };
		trans.scale = glm::vec3(30.0f);
		ffModel.Load("dog", trans);

		//ffmodel_buffer.UpdateDescriptor(sizeof(Triangle) * ffModel.triangles.size());
		ffmodel_buffer.CopyMem(ffModel.triangles.data(), sizeof(Triangle) * ffModel.triangles.size());

		TriangleUBO triUBO{};

		triUBO.triangle_count = ffModel.triangles.size();
		triUBO.vertex_per_triangle = 3;
		triUBO.max = ffModel.max;
		triUBO.min = ffModel.min;

		triangle_ubo_buffer.CopyMem(&triUBO, sizeof(triUBO));
	}
	else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_G))
	{
		Transform trans;
		trans.position = { 0.0f, 0.0f, 0.0f };
		trans.scale = glm::vec3(30.0f);
		ffModel.Load("heart", trans);

		//ffmodel_buffer.UpdateDescriptor(sizeof(Triangle) * ffModel.triangles.size());
		ffmodel_buffer.CopyMem(ffModel.triangles.data(), sizeof(Triangle) * ffModel.triangles.size());

		TriangleUBO triUBO{};

		triUBO.triangle_count = ffModel.triangles.size();
		triUBO.vertex_per_triangle = 3;
		triUBO.max = ffModel.max;
		triUBO.min = ffModel.min;

		triangle_ubo_buffer.CopyMem(&triUBO, sizeof(triUBO));
	}
	else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_F))
	{
		Transform trans;
		trans.position = { 0.0f, 0.0f, 0.0f };
		trans.scale = glm::vec3(30.0f);
		ffModel.Load("bunny", trans);

		//ffmodel_buffer.UpdateDescriptor(sizeof(Triangle) * ffModel.triangles.size());
		ffmodel_buffer.CopyMem(ffModel.triangles.data(), sizeof(Triangle) * ffModel.triangles.size());

		TriangleUBO triUBO{};

		triUBO.triangle_count = ffModel.triangles.size();
		triUBO.vertex_per_triangle = 3;
		triUBO.max = ffModel.max;
		triUBO.min = ffModel.min;

		triangle_ubo_buffer.CopyMem(&triUBO, sizeof(triUBO));
	}

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_L))
	{
		flow = !flow;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_1))
	{
		particle_ubo.flowType = 1;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_2))
	{
		particle_ubo.flowType = 2;
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_3))
	{
		particle_ubo.flowType = 3;
	}

	auto dt = Locator::GetTimer()->DeltaTime();
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_RIGHT))
	{
		uboLight.roughness += dt * dt;
		uboLight.roughness = std::min(uboLight.roughness, 1.0f);
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_LEFT))
	{
		uboLight.roughness -= dt * dt;
		uboLight.roughness = std::max(uboLight.roughness, 0.00f);
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_UP))
	{
		uboLight.metallic += dt * dt;
		uboLight.metallic = std::min(uboLight.metallic, 1.0f);
	}
	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_DOWN))
	{
		uboLight.metallic -= dt * dt;
		uboLight.metallic = std::max(uboLight.metallic, 0.00f);
	}


	//particleLightBuffer.CopyMem(lights.data(), sizeof(Light) * lights.size());

	//CheckParticles();

	//DoShit();
	particle_system.Update();
	//std::string pos = Utillities::V3ToString("P" + std::to_string(0), particle_system.Particles(0).position);
	//DebugPrinter::Print(pos);

	uboLight.camPos = camera.GetTransform().position;
	uboLight.lightCount = particle_system.Particles().size();
	uboLight.frameNum = static_cast<int>(currentFrame);
	lightUboBuffer.CopyMem(&uboLight, sizeof(LightUBO), 0);
	//uboLight.lightCount = particle_system.ParticleCount();

	//lgh.Recreate(lights);

	//lgh.Recreate(&lights);

	//for (int i = 0; i < lights.size(); ++i)
	//{
	//	std::string pos = Utillities::V3ToString("P" + std::to_string(i), lights[i].pos);
	//	DebugPrinter::Print(pos);
	//}

	//uboLight.lightCount = lgh.Lights().size();

	//lightUboBuffer.CopyMem(&uboLight, sizeof(LightUBO));
	//lightBuffer.CopyMem(lights.data(), sizeof(Light) * lights.size());
	
	//lightBuffer.UpdateDescriptor(sizeof(Light) * lgh.Lights().size());
	//lightBuffer.CopyMem(lgh.Lights().data(), sizeof(Light) * lgh.Lights().size());


	//frameCount++;
	//if (frameCount > frameSkipCount) frameCount = 0;

	//object.Update();
	camera.Update();
}

void Scene::drawFrame()
{

	if (framebufferResized)
	{
		//vkWaitForFences(device, 1, &compute.fences[currentFrame], VK_TRUE, UINT64_MAX);
		//vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);
		framebufferResized = false;
		RecreateSwapChain();
	}

	Locator::GetMouse()->Update();
	uint32_t imageIndex;
	prepareFrame(imageIndex);

	Update();

	updateUniformBuffer(imageIndex);

	//output.Update();

	endFrame(imageIndex);

	vkWaitForFences(device, 1, &compute.fences[currentFrame], VK_TRUE, UINT64_MAX);
	vkResetFences(device, 1, &compute.fences[currentFrame]);

	//VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_VERTEX_INPUT_BIT };

	VkSubmitInfo computeSubmitInfo{};
	computeSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	computeSubmitInfo.commandBufferCount = 1;
	computeSubmitInfo.pCommandBuffers = &compute.commandBuffers[imageIndex];
	//computeSubmitInfo.pWaitDstStageMask = waitStages;
	//computeSubmitInfo.waitSemaphoreCount = 1;
	//computeSubmitInfo.pWaitSemaphores = &graphicsSemaphore;

	//computeSubmitInfo.pSignalSemaphores = &compute.semaphore;
	//computeSubmitInfo.signalSemaphoreCount = 1;

	if (vkQueueSubmit(compute.queue, 1, &computeSubmitInfo, compute.fences[currentFrame]) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit compute command buffer!");
	}

	//std::string ym = "Current Frame : " + std::to_string(currentFrame) + " | Image Index : " + std::to_string(imageIndex);
	//DebugPrinter::Print(ym);

	currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Scene::Cleanup()
{
	CleanUpObjects();

	cleanupCompute();
	CTPApp::cleanup();
}

void Scene::CleanUpObjects()
{
	particle_system.Destroy();
	object.Destroy();

	particle_ubo_buffer.DestoryBuffer();
	lightUboBuffer.DestoryBuffer();
	//lightBuffer.DestoryBuffer();
	ffmodel_buffer.DestoryBuffer();
	triangle_ubo_buffer.DestoryBuffer();
	vkDestroyPipeline(device, objectPipeline, nullptr);
	vkDestroyPipeline(device, pSystemPipeline, nullptr);
}