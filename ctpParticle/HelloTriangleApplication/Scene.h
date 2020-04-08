#pragma once

#include "App.h"
#include "Constants.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"
#include "LGH.h"

struct LightUBO {
	glm::vec3 camPos;
	int lightCount;
};

struct ParticleUBO {
	float delta_time;
	int particle_count;
};

class Scene : public CTPApp
{
public:
	Scene() = default;
	~Scene();

	void run();

	void LocatorSetup();

private:

	ParticleSystem particle_system;
	Camera camera;

	GameObject object;
	VkDescriptorSet objectDescSet;
	VkPipeline objectPipeline;

	VkDescriptorSet pSystemDescSet;

	VkPipeline pSystemPipeline;

	glm::mat4 perspective;

	std::vector<Triangle> nearestTri;

	FfObject ffModel;

	Buffer particle_ubo_buffer;

	Buffer lightBuffer;
	Buffer lightUboBuffer;

	std::vector<Light> lights;
	LightUBO uboLight;

	LGH lgh;

	float rotTime = 0.0f;

	void mainLoop();

	void createCommandBuffers();

	void GetComputeQueue();

	void createCompute();

	void BuildComputeCommandBuffer();

	void createDescriptorPool();

	void createDescriptorSetLayout();

	void createDescriptorSets();

	void createGraphicsPipeline();

	glm::vec3 getFlowField(glm::vec3 pos);

	void createUniformBuffers();

	void updateUniformBuffer(uint32_t currentImage);

	void LoadAssets();

	void drawFrame();

	void Update(size_t currentImage);

	void Update();

	void CheckParticles();

	void GetClosestTri();

	void GetClosestTri(size_t i);

	void Cleanup();
};

