#pragma once

#include "App.h"

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

	int pointCount = 1000;
	std::vector<Vertex> points;
	std::vector<Buffer> uniformPoints;
	std::vector<Buffer> vertexs;
	std::vector<VkDescriptorSet> pointDescSets;

	VkPipeline pointPipeline;
	Texture pointTexture;

	float camSpeed = 50.0f;
	float distFromOrigin = 30.0f;
	glm::vec3 camPos = glm::vec3(0, 3.0f, -30.0f);
	float angleX = 0.0f;
	float angleY = 0.0f;
	float angleSpeed = 2.5f;

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

	void drawFrame();

	void Update();

};

