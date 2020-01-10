#include "Scene.h"
#include "Locator.h"
#include "Keyboard.h"
#include "Timer.h"

#include <chrono>
#include <iostream>

Scene::~Scene()
{
}

void Scene::Init(VkPhysicalDevice* _phyDevice, VkDevice* _device, GLFWwindow* _window, VkQueue* gQueue, VkQueue* pQueue, Graphics* _graphics)
{
	physicalDevice = _phyDevice;
	device = _device;
	window = _window;
	graphicsQueue = gQueue;
	presentQueue = pQueue;
	graphics = _graphics;
}

void Scene::Update(uint32_t currentImage)
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


	UpdateUniformBuffers(currentImage);
}

void Scene::Render()
{
}

void Scene::CreateUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(graphics->GetSwapChain().swapChainImages.size());
	uniformBuffersMemory.resize(graphics->GetSwapChain().swapChainImages.size());

	for (size_t i = 0; i < graphics->GetSwapChain().swapChainImages.size(); i++) {
		VkHelper::createBufferWithoutStaging(*physicalDevice, *device,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize,
			uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void Scene::UpdateUniformBuffers(uint32_t currentImage) {
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(0.0f), glm::vec3(time * glm::radians(90.0f), 1.0f, time * glm::radians(90.0f)));
	ubo.view = glm::lookAt(camPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 1000.0f);
	ubo.proj[1][1] *= -1;

	VkHelper::copyMemory(*device, sizeof(ubo), uniformBuffersMemory[currentImage], &ubo);
}

void Scene::createDescriptorSets() {

	//std::vector<VkDescriptorSetLayout> layouts(graphics->GetSwapChain().swapChainImages.size(), graphicsData.descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(graphics->GetSwapChain().swapChainImages.size());
	//allocInfo.pSetLayouts = layouts.data();

	//particleSysDescs.resize(graphics.GetSwapChain().swapChainImages.size());
	if (vkAllocateDescriptorSets(*device, &allocInfo, &particleSysDesc) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < graphics->GetSwapChain().swapChainImages.size(); i++) {

		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		//imageInfo.imageView = textureData.textureImageView;
		//imageInfo.sampler = textureData.textureSampler;

		std::array<VkWriteDescriptorSet, 2> descriptorWrites = {};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = particleSysDesc;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = particleSysDesc;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(*device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
