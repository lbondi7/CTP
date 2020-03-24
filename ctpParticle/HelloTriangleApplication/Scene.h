#pragma once

#include "App.h"
#include "Constants.h"
#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"
#include "LGH.h"

struct LightUBO {
	glm::vec3 camPos;
	int particleCount;
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

	Buffer lightBuffer;
	Buffer lightUboBuffer;

	std::vector<Light> lights;
	LightUBO uboLight;

	LGH lgh;

	float rotTime = 0.0f;

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

	void DisplayLights();

	static void CalculateLights(int start, int end);

	void CalculateLights();

	glm::vec4 CalInternalLight(int i, const glm::vec3& lightDirection, const glm::vec3& Normal, const glm::vec4& pos);

	//glm::vec4 CalInternalLight(int i, glm::vec3 lightDirection, glm::vec3 Normal, glm::vec4 pos);

	//glm::vec4 CalInternalLight(int i, const glm::vec3& lightDirection, const glm::vec3& Normal, const glm::vec4& pos);

	void CheckParticles();

	void createLight();

	void GetClosestTri();

	void GetClosestTri(size_t i);

	void updateLight();

	bool checkDistanceFromLight(glm::vec3 pos, int i);

	void Cleanup();
};

