#pragma once

#include "App.h"

#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"

struct Light
{
	glm::vec3 pos;
	float radius;
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
	VkPipeline objectPipeline;

	VkDescriptorSet pSystemDescSet;

	VkPipeline pSystemPipeline;

	glm::mat4 perspective;

	std::vector<Triangle> nearestTri;

	FfObject ffModel;

	Light light;

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

	void createLight();

	void GetClosestTri();

	void updateLight();

	bool checkDistanceFromLight(glm::vec3 pos, int i);

	void Cleanup();
};

