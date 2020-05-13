#pragma once

#include "App.h"
#include "Constants.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"
#include "LGH.h"
#include "KdTree.h"

struct LightUBO {
	alignas(16) glm::vec3 camPos;
	alignas(4) int lightCount;
};

struct ParticleUBO {
	float delta_time = 0.0f;
	int particle_count = 0;
	glm::vec2 resolution = {WIDTH, HEIGHT};
	glm::vec2 randomVec = { 1.0, 1.0 };
};

struct TriangleUBO {
	int triangle_count = 0;
	int vertex_per_triangle = 0;
	alignas(16) glm::vec4 min = { 0, 0, 0, 0 };
	alignas(16) glm::vec4 max = { 0, 0, 0, 0 };
};

class Scene : public CTPApp
{
public:
	Scene() = default;
	~Scene();

	void run();

	void RecreateSwapChain();

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

	Buffer triangle_ubo_buffer;

	Buffer ffmodel_buffer;

	Buffer lightBuffer;

	Buffer lightUboBuffer;

	Buffer particleLightBuffer;

	std::vector<Light> lights;
	LightUBO uboLight;

	LGH lgh;

	KdTree kdTree;

	int frameSkipCount = 0;
	int frameCount = 0;

	void mainLoop();

	void createCommandBuffers();

	void GetComputeQueue();

	void createCompute();

	void BuildComputeCommandBuffer();

	void createDescriptorPool();

	void createDescriptorSetLayout();

	void createPipelineLayout();

	void createDescriptorSets();

	void createGraphicsPipeline();

	glm::vec3 getFlowField(glm::vec3 pos);

	void createUniformBuffers();

	void updateUniformBuffer(uint32_t currentImage);

	void LoadAssets();

	void drawFrame();

	void Update();

	//void UpdateParticleBehaviour();

	void Cleanup();
	void CleanUpObjects();
};

