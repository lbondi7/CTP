#include "Scene.h"
#include "Locator.h"
#include "Keyboard.h"
#include "Timer.h"

#include <chrono>
#include <iostream>

Scene::~Scene()
{
}

void Scene::Init(VkPhysicalDevice* _phyDevice, VkDevice* _device, GLFWwindow* _window, VkQueue* gQueue, VkQueue* pQueue)
{
	physicalDevice = _phyDevice;
	device = _device;
	window = _window;
	graphicsQueue = gQueue;
	presentQueue = pQueue;
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


	//UpdateUniformBuffers(currentImage);
}

void Scene::Render()
{
}

void Scene::CreateUniformBuffers() {
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	//uniformBuffers.resize(graphics->GetSwapChain().swapChainImages.size());
	//uniformBuffersMemory.resize(graphics->GetSwapChain().swapChainImages.size());

	//for (size_t i = 0; i < graphics->GetSwapChain().swapChainImages.size(); i++) {
	//	VkHelper::createBufferWithoutStaging(*physicalDevice, *device,
	//		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, bufferSize,
	//		uniformBuffers[i], uniformBuffersMemory[i]);
	//}
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

	//VkHelper::copyMemory(*device, sizeof(ubo), uniformBuffersMemory[currentImage], &ubo);
}
