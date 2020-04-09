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
//#include "Utillities.h"

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

using namespace std::chrono_literals;

Scene::~Scene()
{

}

void Scene::run() {

	initWindow();
	initVulkan();
	LocatorSetup();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	LoadAssets();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
	createCommandBuffers();
	mainLoop();
	Cleanup();
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

	Locator::GetDevices()->CreateCommandPool(commandPool);
}

void Scene::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Locator::GetTimer()->GetTimePoint((float)glfwGetTime());
		drawFrame();
		
		if(updateDelay > 0)
			updateDelay--;

		std::this_thread::sleep_for(1ms);
	}
	vkDeviceWaitIdle(device);
}

void Scene::createDescriptorPool() {

	std::vector<VkDescriptorPoolSize> poolSizes = {
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2),
    VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
	};

	VkDescriptorPoolCreateInfo poolInfo = VkHelper::createDescriptorPoolInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(),
		4);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Scene::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkHelper::createDescriptorLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);

	VkDescriptorSetLayoutBinding samplerLayoutBinding = VkHelper::createDescriptorLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	VkDescriptorSetLayoutBinding lightLayoutBinding = VkHelper::createDescriptorLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);
	
	VkDescriptorSetLayoutBinding lightUboLayoutBinding = VkHelper::createDescriptorLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, samplerLayoutBinding, lightLayoutBinding, lightUboLayoutBinding };
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
	VkHelper::writeDescSet(pSystemDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &pSystem.UBuffer().descriptor),
	VkHelper::writeDescSet(pSystemDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &pSystem.PsTexture().descriptor)
	};

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	if (vkAllocateDescriptorSets(device, &allocInfo, &objectDescSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	descriptorWrites = {
	VkHelper::writeDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &object.GetModel().uniform.descriptor),
	VkHelper::writeDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &object.GetTexture().descriptor),
	VkHelper::writeDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2, &lightBuffer.descriptor),
	VkHelper::writeDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3, &lightUboBuffer.descriptor),
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

	VkViewport viewport = VkHelper::createViewport(0.0f, 0.0f, (float)swapchain.extent.width, (float)swapchain.extent.height, 0.0f, 1.0f);

	//VkViewport viewport = VkHelper::createViewport(0, 0, swapchain.swapChainExtent.width, swapchain.swapChainExtent.height, 0.0f, 1.0f);
	VkRect2D scissor = VkHelper::createScissorRect({ 0, 0 }, swapchain.extent);

	VkPipelineViewportStateCreateInfo viewportState = VkHelper::createViewPortStateInfo(1, 1, &viewport, &scissor);

	VkPipelineRasterizationStateCreateInfo rasterizer = VkHelper::createRasteriser(
		VK_FALSE, VK_FALSE,
		VK_POLYGON_MODE_FILL, 1.0f,
		VK_CULL_MODE_BACK_BIT,
		VK_FRONT_FACE_COUNTER_CLOCKWISE,
		VK_TRUE);


	VkPipelineMultisampleStateCreateInfo multisampling =
		VkHelper::createMultiSampling(VK_FALSE, VK_SAMPLE_COUNT_1_BIT);

	VkPipelineColorBlendAttachmentState colorBlendAttachment =
		VkHelper::createColourBlendAttachment(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, VK_FALSE);

	//colorBlendAttachment.blendEnable = VK_TRUE;
	//colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	//colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	//colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	//colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	//colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	//colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = VkHelper::createColourBlendStateInfo(
		VK_FALSE, VK_LOGIC_OP_COPY, 1, &colorBlendAttachment, { 0.0f, 0.0f, 0.0f, 0.0f });

	VkPipelineDepthStencilStateCreateInfo depthStencil = VkHelper::createDepthStencilInfo(
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

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = Locator::GetShader()->CreateShaderInfo("basicAVert", VK_SHADER_STAGE_VERTEX_BIT);
		//VkHelper::createShaderStageInfo("shaders/basicA/basicAVert.spv", VK_SHADER_STAGE_VERTEX_BIT, device);

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = Locator::GetShader()->CreateShaderInfo("basicAFrag", VK_SHADER_STAGE_FRAGMENT_BIT);
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

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkDeviceSize offsets[] = { 0 };

	/*	for (size_t j = 0; j < lightCount; j++)
		{
			uint32_t dynamicOffset = j * static_cast<uint32_t>(dynamicAlignment);
		}*/
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &objectDescSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &object.GetModel().vertex.buffer, offsets);
		//vkCmdBindVertexBuffers(commandBuffers[i], 1, 1, &lightCountBuffer.buffer, offsets);
		vkCmdBindIndexBuffer(commandBuffers[i], object.GetModel().index.buffer, 0, VK_INDEX_TYPE_UINT32);
		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(object.GetModel().indices.size()), 1, 0, 0, 0);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &pSystemDescSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pSystemPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &pSystem.PBuffer().buffer, offsets);
		vkCmdDraw(commandBuffers[i], pSystem.ParticleCount(), 1, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void Scene::createCompute() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkHelper::createDescriptorLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	VkDescriptorSetLayoutBinding storageLayoutBinding = VkHelper::createDescriptorLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_COMPUTE_BIT);

	std::vector<VkDescriptorSetLayoutBinding> bindings = { uboLayoutBinding, storageLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo = VkHelper::createDescSetLayoutInfo(static_cast<uint32_t>(bindings.size()), bindings.data());

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
       //VkHelper::writeDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 0, &),
       //VkHelper::writeDescSet(compute.descriptorSet, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, &)
	};

	vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);

	VkComputePipelineCreateInfo computePipelineCreateInfo{};
	computePipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
	computePipelineCreateInfo.layout = compute.pipelineLayout;
	computePipelineCreateInfo.flags = 0;

	//VkPipelineShaderStageCreateInfo vertShaderStageInfo = Locator::GetShader()->CreateShaderInfo("pointSpriteComp", VK_SHADER_STAGE_COMPUTE_BIT);

	computePipelineCreateInfo.stage = Locator::GetShader()->CreateShaderInfo("pointSpriteComp", VK_SHADER_STAGE_COMPUTE_BIT);
	if (vkCreateComputePipelines(device, VK_NULL_HANDLE, 1, &computePipelineCreateInfo, nullptr, &compute.pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create compute pipeline!");
	}
	Locator::GetShader()->DestroyShaders();
}

glm::vec3 Scene::getFlowField(glm::vec3 pos)
{
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, -pos.x * pos.y) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z)));
	glm::vec3 vel = glm::vec3(-pos.y, pos.x, pos.x) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z));
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, 0) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y)));
	//vel.x = std::sqrt((pos.x * pos.x) + (pos.y * pos.y));
	//vel.y = 0;
	//vel.z = 0;

	return vel;
}

void Scene::createUniformBuffers() 
{
	lights.resize(pSystem.ParticleCount());

	for (size_t i = 0; i < pSystem.ParticleCount(); i++)
	{
		//lights[i].col = glm::vec3(255.0f / 255.0f, 0.0f / 255.0f, 0.0f / 255.0f);
		lights[i].col = glm::vec3(255.0f / 255.0f, 0.0f / 255.0f, 255.0f / 255.0f);
		lights[i].pos = pSystem.PsParticle(i).position;
	}

	lightBuffer.CreateBuffer(VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Light) * lights.size());
	lightBuffer.StageBuffer(lightBuffer.size, graphicsQueue, lights.data(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	lightBuffer.UpdateDescriptor(sizeof(Light) * lights.size());

	uboLight.camPos = camera.GetTransform().pos;
	uboLight.particleCount = pSystem.ParticleCount();

	lightUboBuffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(LightUBO));

	lightUboBuffer.UpdateDescriptor(sizeof(LightUBO));

	object.GetModel().uniform.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));

	object.GetModel().uniform.UpdateDescriptor(sizeof(UniformBufferObject));
}

void Scene::updateUniformBuffer(uint32_t currentImage) {

	UniformBufferObject ubo = {};

	//object.GetModel().transform.pos += getFlowField(object.GetModel().transform.pos) * Locator::GetTimer()->DeltaTime();

	rotTime *= Locator::GetTimer()->DeltaTime();

	//ubo.model = glm::scale(glm::mat4(1.0f), object.GetTransform().scale) *
	//	glm::rotate(glm::mat4(1.0f), rotTime, glm::vec3(12, 12, 1)) *

	ubo.model =	glm::translate(glm::mat4(1.0f), object.GetTransform().pos);
	ubo.model = glm::scale(ubo.model, object.GetTransform().scale);
	ubo.view = camera.ViewMatrix();
	ubo.proj = perspective;

	object.GetModel().uniform.CopyMem(&ubo, sizeof(ubo));
}

void GetNearestTri(size_t i, std::vector<Triangle>* nearestTri, FfObject* ffModel, ParticleSystem* pSystem)
{
	if ((*pSystem).PsParticle(i).goToTri)
		return;

	std::mutex mut;
	float nearestPoint = INFINITY;
	float nP = INFINITY;
	for (size_t j = 0; j < (*ffModel).triangles.size(); ++j)
	{
		if (j == 0)
		{
			nearestPoint = ffModel->triangles[j].udTriangle((*pSystem).PsParticle(i).position);
			std::lock_guard<std::mutex> lock(mut);
			(*nearestTri)[i] = (*ffModel).triangles[j];
		}
		else
		{
			nP = (*ffModel).triangles[j].udTriangle((*pSystem).PsParticle(i).position);
			if (nearestPoint > nP)
			{
				nearestPoint = nP;
				std::lock_guard<std::mutex> lock(mut);
				(*nearestTri)[i] = (*ffModel).triangles[j];
			}
		}
	}
}

void FindTri(std::vector<Triangle>* nearestTri, FfObject* ffModel, ParticleSystem* pSystem)
{
	std::mutex mut;
	bool first = true;

	while (true)
	{
		for (size_t i = 0; i < pSystem->ParticleCount(); ++i)
		{
			GetNearestTri(i, nearestTri, ffModel, pSystem);
		}
		if (first)
		{
			for (size_t i = 0; i < pSystem->ParticleCount(); ++i)
			{
				std::lock_guard<std::mutex> lock(mut);
				(*pSystem).PsParticle(i).goToTri = true;
			}
			first = false;
		}
		std::this_thread::sleep_for(1ms);
	}

}

void Scene::LoadAssets()
{
	perspective = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
	perspective[1][1] *= -1;

	camera.Setup(glm::vec3(-10, 3.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), false);

	pSystem.Create(graphicsQueue, &camera.ViewMatrix(), &perspective);

	nearestTri.resize(pSystem.ParticleCount());

	object.Init("bunny", "blank", graphicsQueue);

	for (int i = 0; i < object.GetModel().indices.size(); i += 3)
	{
		glm::vec3 v1 = object.GetModel().vertices[object.GetModel().indices[i + 1]].pos - 
			object.GetModel().vertices[object.GetModel().indices[i]].pos;
		glm::vec3 v2 = object.GetModel().vertices[object.GetModel().indices[i + 2]].pos -
			object.GetModel().vertices[object.GetModel().indices[i]].pos;
		glm::vec3 normal = glm::cross(v1, v2);

		normal = glm::normalize(normal);
		object.GetModel().vertices[object.GetModel().indices[i]].normal += normal;
		object.GetModel().vertices[object.GetModel().indices[i + 1]].normal += normal;
		object.GetModel().vertices[object.GetModel().indices[i + 2]].normal += normal;


		//object.GetModel().vertices[object.GetModel().indices[i]].normal = glm::normalize(object.GetModel().vertices[object.GetModel().indices[i]].normal);
		//object.GetModel().vertices[object.GetModel().indices[i + 1]].normal = glm::normalize(object.GetModel().vertices[object.GetModel().indices[i]].normal);
		//object.GetModel().vertices[object.GetModel().indices[i + 2]].normal = glm::normalize(object.GetModel().vertices[object.GetModel().indices[i]].normal);

		//object.GetModel().vertices[object.GetModel().indices[i]].normal = normal;
		//object.GetModel().vertices[object.GetModel().indices[i + 1]].normal = normal;
		//object.GetModel().vertices[object.GetModel().indices[i + 2]].normal = normal;


	}

	Transform transform;
	transform.pos = { 0.0f, 0.0f, 0.0f };
	transform.scale = { 1.0f, 1.0f, 1.0f };
	//transform.scale = { 4.0f, 4.0f, 4.0f };

	object.SetTransform(transform);

	Transform trans;
	trans.pos = { 0.0f, 0.0f, 0.0f };
	trans.scale = { 1.0f, 1.0f, 1.0f };
	ffModel.Load("bunny", trans);

	GetClosestTri();

//	std::thread th(FindTri, &nearestTri, &ffModel, &pSystem);
//	th.detach();
}

void Scene::Update()
{

	if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_SPACE))
	{
		for (size_t i = 0; i < pSystem.ParticleCount(); i++)
		{
			pSystem.PsParticle(i).goToTri = false;
			pSystem.PsParticle(i).ranDirDuration = 4.0f;

			std::random_device rd;
			std::uniform_real_distribution<float> rand(-5.0f, 5.0f);
			pSystem.PsParticle(i).velocity = { rand(rd), rand(rd), rand(rd) };
		}
	}
	else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_H))
	{
		for (size_t i = 0; i < pSystem.ParticleCount(); i++)
		{
			pSystem.PsParticle(i).goToTri = false;
			pSystem.PsParticle(i).ranDirDuration = 4.0f;

			std::random_device rd;
			std::uniform_real_distribution<float> rand(0.0f, 5.0f);

			pSystem.PsParticle(i).velocity = glm::normalize(pSystem.PsParticle(i).position - ffModel.GetTransform().pos) * rand(rd);
		}
	}
	else if (Locator::GetKeyboard()->IsKeyPressed(GLFW_KEY_J))
	{
		for (size_t i = 0; i < pSystem.ParticleCount(); i++)
		{
			pSystem.PsParticle(i).goToTri = false;
			pSystem.PsParticle(i).ranDirDuration = 4.0f;

			std::random_device rd;
			std::uniform_real_distribution<float> rand(0.0f, 5.0f);

			pSystem.PsParticle(i).velocity = glm::normalize(ffModel.GetTransform().pos - pSystem.PsParticle(i).position) * rand(rd);
		}
	}

	CheckParticles();

	pSystem.Update();

	for (size_t i = 0; i < pSystem.ParticleCount(); i++)
	{
		lights[i].pos = pSystem.PsParticle(i).position;
		//std::cout << lights[i].pos.x << ", " << lights[i].pos.y << ", " << lights[i].pos.z << std::endl;
	}
	uboLight.camPos = camera.GetTransform().pos;
	uboLight.particleCount = pSystem.ParticleCount();

	lightUboBuffer.CopyMem(&uboLight, sizeof(LightUBO));

	//lgh.Recreate(lights);

	//lightBuffer.UpdateDescriptor(sizeof(Light) * lgh.Lights().size());
	//lightBuffer.CopyMem(lgh.Lights().data(), sizeof(Light) * lgh.Lights().size());
	lightBuffer.CopyMem(lights.data(), sizeof(Light) * lights.size());

	//DisplayLights();

	object.Update();

	camera.Update();
}

void Scene::CheckParticles()
{
	float length, length2;
	for (size_t i = 0; i < pSystem.ParticleCount(); i++)
	{
		if (pSystem.PsParticle(i).goToTri)
		{
			length = glm::distance(pSystem.PsParticle(i).position, pSystem.PsParticle(i).target);
			length2 = glm::distance(pSystem.PsParticle(i).target + (nearestTri[i].normal), pSystem.PsParticle(i).target);

			//std::cout << "Lengths: " <<length << ", " << length2 << std::endl;
			if (length < length2)
			{
				std::random_device rd;
				//std::uniform_real_distribution<float> rand(-1.0f, 1.0f);
				std::uniform_real_distribution<float> rand2(0.005f, 0.01f);
				pSystem.PsParticle(i).goToTri = false;
				pSystem.PsParticle(i).ranDirDuration = rand2(rd);
				pSystem.PsParticle(i).velocity = { 0.0f, 0.0f, 0.0f };
				//pSystem.PsParticle(i).velocity = { rand(rd), rand(rd), rand(rd) };
				//std::cout << "Random Position: " << pSystem.PsParticle(i).ranDirDuration << std::endl;
			}
		}
		else
		{
			if (pSystem.PsParticle(i).ranDirDuration <= 0.0f)
			{
				//std::cout << "Go To Tri: " << pSystem.PsParticle(i).ranDirDuration << std::endl;
				GetClosestTri(i);
				//pSystem.PsParticle(i).target = nearestTri[i].center;
				//pSystem.SetParticleVelocityFromTarget(i, nearestTri[i].center);
				pSystem.PsParticle(i).goToTri = true;
			}
			pSystem.PsParticle(i).ranDirDuration -= Locator::GetTimer()->DeltaTime();
		}
	}
}

void GetTri(int minVal, int maxVal, std::vector<Triangle>* nearestTri, FfObject* ffModel, ParticleSystem* pSystem)
{
	float nearestPoint = INFINITY;
	float nP = INFINITY;

	for (size_t i = minVal; i < maxVal; ++i)
	{
		for (size_t j = 0; j < (*ffModel).triangles.size(); ++j)
		{
			if (j == 0)
			{
				nearestPoint = (*ffModel).triangles[j].udTriangle((*pSystem).PsParticle(i).position);
				(*nearestTri)[i] = (*ffModel).triangles[j];
				(*pSystem).SetParticleVelocityFromTarget(i, (*nearestTri)[i].center);
			}
			else
			{
				nP = (*ffModel).triangles[j].udTriangle((*pSystem).PsParticle(i).position);
				if (nearestPoint > nP)
				{
					nearestPoint = nP;
					(*nearestTri)[i] = (*ffModel).triangles[j];
					(*pSystem).SetParticleVelocityFromTarget(i, (*nearestTri)[i].center);
				}
			}
		}
		(*pSystem).PsParticle(i).goToTri = true;
	}
}

void Scene::GetClosestTri()
{

	//float nearestPoint = INFINITY;
	//float nP = INFINITY;

	if (pSystem.ParticleCount() < 4)
	{
		GetTri(0, pSystem.ParticleCount(), & nearestTri, & ffModel, & pSystem);
		return;
	}

	int interval = pSystem.ParticleCount() / 4;

	std::thread threads[4];

	int start_count = 0;
	for (size_t i = 0; i < 4; ++i)
	{
		threads[i] = std::thread(GetTri, start_count, start_count + interval, &nearestTri, &ffModel, &pSystem);

		if (interval == pSystem.ParticleCount())
			break;

		if (start_count + interval > pSystem.ParticleCount())
		{
			start_count += pSystem.ParticleCount() - start_count;
		}
		else
		{
			start_count += interval;
		}
	}

	for (size_t i = 0; i < 4; ++i)
	{
		if(threads[i].joinable())
			threads[i].join();
	}


	int x = 0;

	//for (size_t i = 0; i < pSystem.ParticleCount(); i += interval)
	//{
	//	if (i + interval > pSystem.ParticleCount())
	//		interval = pSystem.ParticleCount() - i;

	//	std::thread th(GetTri, i, i + interval, &nearestTri, &ffModel, &pSystem);

	//	if (i > pSystem.ParticleCount() - (interval + 10))
	//		th.join();
	//	else
	//		th.detach();
	//}

	//float nearestPoint = INFINITY;
	//float nP = INFINITY;
	//for (size_t i = 0; i < pSystem.ParticleCount(); ++i)
	//{
	//	for (size_t j = 0; j < ffModel.triangles.size(); ++j)
	//	{
	//		if (j == 0)
	//		{
	//			nearestPoint = ffModel.triangles[j].udTriangle(pSystem.PsParticle(i).position);
	//			nearestTri[i] = ffModel.triangles[j];
	//		}
	//		else
	//		{
	//			nP = ffModel.triangles[j].udTriangle(pSystem.PsParticle(i).position);
	//			if (nearestPoint > nP)
	//			{
	//				nearestPoint = nP;
	//				nearestTri[i] = ffModel.triangles[j];
	//				pSystem.PsParticle(i).target = nearestTri[i].center;
	//				pSystem.SetNewTarget(i, nearestTri[i].center);
	//				pSystem.PsParticle(i).goToTri = true;
	//			}
	//		}
	//	}
	//}
}

void Scene::GetClosestTri(size_t i)
{
	float nearestPoint = INFINITY;
	float nP = INFINITY;
	for (size_t j = 0; j < ffModel.triangles.size(); ++j)
	{
		if (j == 0)
		{
			nearestPoint = ffModel.triangles[j].udTriangle(pSystem.PsParticle(i).position);
			nearestTri[i] = ffModel.triangles[j];
			pSystem.SetParticleVelocityFromTarget(i, FindRandomPoint(ffModel.triangles[j]));
		}
		else
		{
			nP = ffModel.triangles[j].udTriangle(pSystem.PsParticle(i).position);
			if (nearestPoint > nP)
			{
				nearestPoint = nP;
				nearestTri[i] = ffModel.triangles[j];
				pSystem.SetParticleVelocityFromTarget(i, FindRandomPoint(ffModel.triangles[j]));
			}
		}
	}
	pSystem.PsParticle(i).goToTri = true;
}

glm::vec3 Scene::FindRandomPoint(const Triangle& tri)
{
	bool gotPoint = false;
	std::random_device rd;
	std::uniform_real_distribution<float> x_rand(tri.min.x, tri.max.x);
	std::uniform_real_distribution<float> y_rand(tri.min.y, tri.max.y);
	std::uniform_real_distribution<float> z_rand(tri.min.z, tri.max.z);
	glm::vec3 P;
	glm::vec3 C; // vector perpendicular to triangle's plane 
	float denom = glm::dot(tri.normal, tri.normal);
	while (!gotPoint)
	{
		P = glm::vec3(x_rand(rd), y_rand(rd), z_rand(rd));
		// no need to normalize

		glm::vec3 vp0 = P - tri.vertices[0];
		C = glm::cross(tri.edges[0], vp0);
		if (glm::dot(tri.normal, C) < 0) continue; // P is on the right side 

		glm::vec3 vp1 = P - tri.vertices[1];
		C = glm::cross(tri.edges[1], vp1);
		if ((glm::dot(tri.normal, C)) < 0) continue; // P is on the right side 

		glm::vec3 vp2 = P - tri.vertices[2];
		C = glm::cross(tri.edges[2], vp2);
		if ((glm::dot(tri.normal, C)) < 0) continue; // P is on the right side;

		gotPoint = true;

	}

	return P;

	//// no need to normalize
	//float denom = Vec3::Dot(normal, normal);

	//// compute the intersection point using equation 1
	//Vec3 P = ray.origin + t * ray.direction;

	//Vec3 C; // vector perpendicular to triangle's plane 

	//Vec3 vp0 = P - vertices[0].pos;
	//C = Vec3::Cross(edges[0], vp0);
	//if (Vec3::Dot(normal, C) < 0) return false; // P is on the right side 

	//Vec3 vp1 = P - vertices[1].pos;
	//C = Vec3::Cross(edges[1], vp1);
	//if ((u = Vec3::Dot(normal, C)) < 0)  return false; // P is on the right side 

	//Vec3 vp2 = P - vertices[2].pos;
	//C = Vec3::Cross(edges[2], vp2);
	//if ((v = Vec3::Dot(normal, C)) < 0) return false; // P is on the right side; 

	//u /= denom;
	//v /= denom;
}

void Scene::drawFrame() {

	Locator::GetMouse()->Update();
	uint32_t imageIndex;
	prepareFrame(imageIndex);
	/* Updating obejcts*/
	/////////////////////

	if (updateDelay <= 0)
	{
		Update();

		updateUniformBuffer(imageIndex);
	}

	endFrame(imageIndex);
}

void Scene::Cleanup()
{

	vkDestroyPipeline(device, objectPipeline, nullptr);
	vkDestroyPipeline(device, pSystemPipeline, nullptr);

	lightUboBuffer.DestoryBuffer();
	lightBuffer.DestoryBuffer();
	object.Destroy();
	pSystem.Destroy();

	CTPApp::cleanup();
}
