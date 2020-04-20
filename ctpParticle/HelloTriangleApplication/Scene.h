#pragma once

#include "App.h"
#include "Constants.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"
#include "LGH.h"

struct LightUBO {
	alignas(16) glm::vec3 camPos;
	alignas(2) uint16_t lightCount;
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

	Buffer triangle_ubo_buffer;

	Buffer ffmodel_buffer;

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

	//void CheckParticles();

	//void GetClosestTri();

	//void DoShit();

	//void GetClosestTri(size_t i);

	//glm::vec3 FindRandomPoint(const Triangle& tri);

	void Cleanup();
};

