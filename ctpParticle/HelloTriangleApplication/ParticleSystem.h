#pragma once

#include "Object.h"
#include "Buffer.h"
#include "VkHelper.h"
#include "Constants.h"

#include "glm/glm.hpp"
#include <vector>

struct Particle
{
	alignas(16) glm::vec4 position;
	alignas(16) glm::vec4 velocity;
	alignas(4) float alpha = 0.25f;
	alignas(4) bool goToTri = false;
	alignas(4) float ranDirDuration = 0.0f;
	alignas(4) int targetTri = 0;
	alignas(16) glm::vec4 targetPoint = { 0.0f, 0.0f, 0.0f, 1.0f };
	//float life = 0.0f;
	//float maxLife = 10.0f;
	//glm::vec3 target;
	//bool goToTri = false;
	//float ranDirDuration = 0.0f;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(0, sizeof(Particle), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		VkHelper::createVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, position)),
		VkHelper::createVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, velocity)),
		VkHelper::createVertexAttributeDescription(0, 2, VK_FORMAT_R32_SFLOAT, offsetof(Particle, alpha)),
		VkHelper::createVertexAttributeDescription(0, 3, VK_FORMAT_R32_SINT, offsetof(Particle, goToTri)),
		VkHelper::createVertexAttributeDescription(0, 4, VK_FORMAT_R32_SFLOAT, offsetof(Particle, ranDirDuration)),
		VkHelper::createVertexAttributeDescription(0, 5, VK_FORMAT_R32_SINT, offsetof(Particle, targetTri)),
		VkHelper::createVertexAttributeDescription(0, 6, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Particle, targetPoint))
		};

		return attributeDescriptions;
	}

	//bool operator==(const Particle& other) const {
	//	return position == other.position && velocity == other.velocity;
	//}
};

class ParticleSystem : public Object
{
public:

	ParticleSystem() = default;
	ParticleSystem(int _amount, VkQueue graphicsQueue);

	void Create(int _amount, VkQueue graphicsQueue);

	void Create(VkQueue graphicsQueue, const glm::mat4* _view, glm::mat4* _perspective);

	void Update();

	void UpdateBuffers();

	void Destroy();

	int ParticleCount() { return amount; }

	Texture& PsTexture() { return texture; }

	Buffer& PBuffer() { return particleBuffer; }

	Buffer& UBuffer() { return uniformBuffer; }

	std::vector<Particle>& Particles() { return particles; }
	
	const std::vector<Particle>& ConstPsParticles() { return particles; }

	Particle& Particles(int i) { return particles[i]; }

	void SetParticleVelocityFromTarget(int i, const glm::vec3& newDest);


private:

	int amount = 256 * 512;

	std::vector<Particle> particles;

	Buffer particleBuffer;
	Buffer uniformBuffer;
	Buffer computeParticleBuffer;
	Texture texture;

	glm::vec3 pos;

	const glm::mat4* view = nullptr;
	glm::mat4* perspective = nullptr;

	UniformBuffer ubp;

	bool lifeEnabled = false;
};

