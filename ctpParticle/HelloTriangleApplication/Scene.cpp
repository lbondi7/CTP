#include "Scene.h"

#include "VkSetupHelper.h"
#include "VkHelper.h"
#include "VkInitializer.h"
#include "Locator.h"
#include "Timer.h"
#include "Devices.h"
#include "Keyboard.h"
#include "Mesh.h"
#include "Image.h"
#include "Shaders.h"

//#define GLM_FORCE_RADIANS
//#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#include <glm/glm.hpp>


//#define GLM_ENABLE_EXPERIMENTAL
//#include <glm/gtx/hash.hpp>

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

const int WIDTH = 800;
const int HEIGHT = 600;

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
	Locator::GetImage()->Load("particle3");

	Locator::InitShader(new Shaders());

	Locator::GetDevices()->CreateCommandPool(commandPool);
}


void Scene::mainLoop() {

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Locator::GetTimer()->GetTimePoint();
		drawFrame();
	}

	vkDeviceWaitIdle(device);
}

void Scene::createDescriptorPool() {

	std::array<VkDescriptorPoolSize, 2> poolSizes = {
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
	VkHelper::createDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2)
	};

	VkDescriptorPoolCreateInfo poolInfo = VkHelper::createDescriptorPoolInfo(static_cast<uint32_t>(poolSizes.size()), poolSizes.data(),
		2);

	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void Scene::createDescriptorSetLayout() {

	VkDescriptorSetLayoutBinding uboLayoutBinding = VkHelper::createDescriptorLayoutBinding(0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, nullptr, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT);

	VkDescriptorSetLayoutBinding samplerLayoutBinding = VkHelper::createDescriptorLayoutBinding(1, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, nullptr, VK_SHADER_STAGE_FRAGMENT_BIT);

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
	VkHelper::writeDescSet(objectDescSet, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, &object.GetTexture().descriptor)
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
		Vertex::getBindingDescription()
	};

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		Vertex::getAttributeDescriptions()
	};

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
	clearValues[0].color = { 100.0f / 255.0f, 149.0f / 255.0f, 237.0f / 255.0f, 1.0f };
	//clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
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

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &objectDescSet, 0, nullptr);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, objectPipeline);
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, &object.GetModel().vertex.buffer, offsets);
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

	//uniformPoint.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferParticle));

	//uniformPoint.UpdateDescriptor(sizeof(UniformBufferParticle));

	object.GetModel().uniform.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferObject));

	object.GetModel().uniform.UpdateDescriptor(sizeof(UniformBufferObject));
}

void Scene::updateUniformBuffer(uint32_t currentImage) {

	UniformBufferParticle ubp = {};
	ubp.model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	ubp.view = camera.ViewMatrix();
	ubp.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.01f, 1000.0f);
	ubp.proj[1][1] *= -1;

	pSystem.UBuffer().CopyMem(&ubp, sizeof(ubp));

	/* uniformPoint.CopyMem(&ubp, sizeof(ubp));*/

	//MoveVertex();

	UniformBufferObject ubo = {};

	//object.GetModel().transform.pos += getFlowField(object.GetModel().transform.pos) * Locator::GetTimer()->DeltaTime();

	ubo.model = glm::translate(glm::mat4(1.0f), object.Transform().pos) * glm::scale(glm::mat4(1.0f), object.Transform().scale);
	ubo.view = camera.ViewMatrix();
	ubo.proj = glm::perspective(glm::radians(45.0f), (float)WIDTH / (float)HEIGHT, 0.1f, 10000.0f);
	ubo.proj[1][1] *= -1;

	object.GetModel().uniform.CopyMem(&ubo, sizeof(ubo));
}

void Scene::LoadAssets()
{

	camera.Setup(glm::vec3(0, 3.0f, -30.0f), glm::vec3(0, 0.0f, 0.0f));

	pSystem.Create(graphicsQueue);

	nearestTri.resize(pSystem.ParticleCount());



	/*pointTexture.Load("particle2", graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);*/

	//vertex.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
	//	VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Vertex) * pointCount);

	//vertex.StageBuffer(vertex.size, graphicsQueue, points.data());

	object.Init("sphere", "spaceBackground3", graphicsQueue);

	TransformData transform;
	transform.pos = { 0.0f, 4.0f, 0.0f };
	transform.scale = { 2.0f, 2.0f, 2.0f };

	object.Transform(transform);

	ffModel.Load("triangle", glm::vec3(5.0f, 5.0f, 5.0f));

	GetClosestTri();
}

void Scene::MoveVertex()
{
	//for (size_t i = 0; i < pointCount; i++)
	//{
	//	points[i].pos += getFlowField(points[i].pos) * Locator::GetTimer()->DeltaTime() * 100.0f;
	//	//sizes[i] -= Locator::GetTimer()->DeltaTime() * 100.0f;
	//}

	//vertex.CopyMem(points.data(), vertex.size);
	//size.CopyMem(sizes.data(), size.size);
}

void Scene::drawFrame() {

	uint32_t imageIndex;
	prepareFrame(imageIndex);
	/* Updating obejcts*/
	/////////////////////

	Update();
	//updateInstanceBuffer();
	//scene.Update(imageIndex);
	updateUniformBuffer(imageIndex);
	//VkHelper::copyMemory(device, sizeof(ubo), uniformBuffersMemory[currentImage], &ubo);

	endFrame(imageIndex);
}

void Scene::Update()
{
	ffModel.Update();
	pSystem.Update();

	camera.Update();

}

void Scene::createLight()
{

}

void Scene::GetClosestTri()
{
	for (size_t i = 0; i < nearestTri.size(); ++i)
	{
		for (size_t j = 0; j < ffModel.triangles.size(); ++j)
		{
			if (j == 0)
				nearestTri[i] = ffModel.triangles[j];
			else
			{
				if ((pSystem.PsParticle(i).position - nearestTri[i].center).length >
					(pSystem.PsParticle(i).position - ffModel.triangles[j].center).length)
				{
					nearestTri[i] = ffModel.triangles[j];
				}
			}
		}
	}
}

void Scene::updateLight()
{
}

bool Scene::checkDistanceFromLight(glm::vec3 pos, int i)
{
	return glm::distance(pos, lights[i].pos) < lights[i].radius;
}

void Scene::Cleanup()
{

	vkDestroyPipeline(device, objectPipeline, nullptr);
	vkDestroyPipeline(device, pSystemPipeline, nullptr);

	object.Destroy();
	pSystem.Destroy();

	CTPApp::cleanup();
}
