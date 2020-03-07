#pragma once

#include "App.h"

#include "Camera.h"
#include "ParticleSystem.h"
#include "FlowFieldObject.h"


//struct LightShit
//{
//	glm::vec3 col;
//	float intensity;
//	glm::vec3 direction;
//	float diffuseIntensity;
//};

//struct LightShit {
//	glm::vec3 col;
//	float intensity;
//	glm::vec3 direction;
//	float diffuseIntensity;
//	glm::vec3 pos;
//	float constant;
//	float linear;
//	float exponent;
//	glm::vec3 camPos;
//};

struct LightShit {
	std::vector<glm::vec3> col;
	std::vector<float> intensity;
	std::vector < glm::vec3> direction;
	std::vector<float> diffuseIntensity;
	std::vector<float> specIntensity;
	std::vector<float> specPower;
	std::vector < glm::vec3> pos;
	std::vector<float> constant;
	std::vector<float> linear;
	std::vector<float> exponent;
	glm::vec3 camPos;
	int numberOfLights;
};

struct Light
{
	glm::vec3 pos;
	glm::vec3 colour;
	float intesity;
	glm::vec3 direction;
	float dIntensity;
	float constant;
	float linear;
	float exponent;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(1, sizeof(Light), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		VkHelper::createVertexAttributeDescription(1, 4, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Light, pos)),
		VkHelper::createVertexAttributeDescription(1, 5, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Light, colour))
		//VkHelper::createVertexAttributeDescription(1, 6, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Light, atten))
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
	VkPipeline objectPipeline;

	VkDescriptorSet pSystemDescSet;

	VkPipeline pSystemPipeline;

	glm::mat4 perspective;

	std::vector<Triangle> nearestTri;

	FfObject ffModel;

	Light light;
	LightShit lights;
	//Light lights[100];

	Buffer lightBuffer;
	Buffer lightCountBuffer;

	//std::vector<Light> lights;
	int lightCount = 1;

	size_t dynamicAlignment;

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

