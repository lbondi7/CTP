#pragma once

#include "App.h"

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

private:

	Object object;
	VkDescriptorSet objectDescSet;
	VkPipeline objectPipeline;

	Vertex point;
	Buffer uniformPoint;
	Buffer vertex;
	VkDescriptorSet pointDescSet;

	int pointCount = 500000;
	std::vector<Vertex> points;

	VkPipeline pointPipeline;
	Texture pointTexture;

	float camSpeed = 50.0f;
	float distFromOrigin = 30.0f;
	glm::vec3 camPos = glm::vec3(0, 3.0f, -30.0f);
	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleSpeed = 2.5f;

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

	void Update();

	void createLight();

	void updateLight();

	bool checkDistanceFromLight(glm::vec3 pos, int i);

	void Cleanup();

};

