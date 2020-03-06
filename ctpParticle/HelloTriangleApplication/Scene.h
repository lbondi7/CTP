#pragma once

#include "App.h"

#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"

struct Light
{
	glm::vec3 pos;
	glm::vec4 colour;
	glm::vec3 atten;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(1, sizeof(Light), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
					VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Light, pos)),
		VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Light, colour)),
		VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Light, atten))
		};

		return attributeDescriptions;
	}

};

class Scene : public CTPApp
{
public:
	Scene() = default;
	~Scene();

	void run();

	void LocatorSetup();

private:

	ParticleSystem pSystem;
	Camera camera;

	GameObject object;
	VkDescriptorSet objectDescSet;
	VkDescriptorSet lightDescSet;
	VkPipeline objectPipeline;

	VkDescriptorSet pSystemDescSet;

	VkPipeline pSystemPipeline;

	glm::mat4 perspective;

	std::vector<Triangle> nearestTri;

	FfObject ffModel;

	Light light;
	//Light lights[100];

	Buffer lightBuffer;

	std::vector<Light> lights;
	std::vector<float> minDists;
	float minDist = 0.0f;

	void mainLoop();

	void createCommandBuffers();

	void createDescriptorPool();

	void createDescriptorSetLayout();

	void createDescriptorSets();

	void createGraphicsPipeline();

	glm::vec3 getFlowField(glm::vec3 pos);

	void createUniformBuffers();

	void updateUniformBuffer(uint32_t currentImage);

	void LoadAssets();

	void MoveVertex();

	void drawFrame();

	void Update(size_t currentImage);

	void Update();

	void CheckParticles();

	void createLight();

	void GetClosestTri();

	void GetClosestTri(size_t i);

	void updateLight();

	bool checkDistanceFromLight(glm::vec3 pos, int i);

	void Cleanup();
};

