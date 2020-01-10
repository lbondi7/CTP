#pragma once
#include "VkConstants.h"
#include "Vertex.h"
#include "Graphics.h"

//struct InstanceData {
//	glm::vec3 pos;
//	glm::vec3 rot;
//	float scale;
//	uint32_t texIndex;
//
//	static VkVertexInputBindingDescription getBindingDescription() {
//		return VkHelper::createVertexBindingDescription(1, sizeof(InstanceData), VK_VERTEX_INPUT_RATE_INSTANCE);
//	}
//
//	static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions() {
//		std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};
//
//		attributeDescriptions[0] = VkHelper::createVertexAttributeDescription(1, 3, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceData, pos));
//		attributeDescriptions[1] = VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(InstanceData, rot));
//		attributeDescriptions[2] = VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32_SFLOAT, offsetof(InstanceData, scale));
//		attributeDescriptions[3] = VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32_SINT, offsetof(InstanceData, texIndex));
//
//		return attributeDescriptions;
//	}
//};

struct Particle
{
	float maxLife;
	float currentLife;
	glm::vec3 velocity;
	bool active;
};

class Scene
{
public:
	Scene() = default;
	~Scene();

	void Init(VkPhysicalDevice* _phyDevice, VkDevice* _device, GLFWwindow* _window, VkQueue* gQueue, VkQueue* pQueue, Graphics* _graphics);
	void Update(uint32_t currentImage);
	void Render();

	void CreateUniformBuffers();

	void UpdateUniformBuffers(uint32_t currentImage);

	void createDescriptorSets();

	std::vector<VkBuffer> uniformBuffers;

private:

	VkPhysicalDevice* physicalDevice = VK_NULL_HANDLE;
	VkDevice* device = nullptr;

	VkQueue* graphicsQueue = nullptr;
	VkQueue* presentQueue = nullptr;

	GLFWwindow* window = nullptr;

	Graphics* graphics;

	VkDescriptorPool descriptorPool;

	VkDescriptorSetLayout descriptorSetLayout;
	VkDescriptorSet particleSysDesc;

	VkPipelineLayout pipelineLayout;
	std::vector<VkPipeline> particleSysPipe;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;


	std::vector<VkDeviceMemory> uniformBuffersMemory;

	std::vector<InstanceData> instanceData;
	std::vector<float> lifetimes;
	std::vector<float> currentLife;
	std::vector<Particle> particles;

	float camSpeed = 20.0f;
	float distFromOrigin = 30.0f;
	glm::vec3 camPos = glm::vec3(0, 3.0f, -30.0f);
	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleSpeed = 2.5f;
};

