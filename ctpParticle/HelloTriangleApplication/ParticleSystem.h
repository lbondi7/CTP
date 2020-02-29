#pragma once

#include "Object.h"
#include "Buffer.h"
#include "VkHelper.h"

#include "glm//glm.hpp"
#include <vector>

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;

	glm::vec3 destination;

	static VkVertexInputBindingDescription getBindingDescription() {
		return VkHelper::createVertexBindingDescription(0, sizeof(Particle), VK_VERTEX_INPUT_RATE_VERTEX);
	}

	static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		VkHelper::createVertexAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Particle, position)),
		VkHelper::createVertexAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Particle, velocity))
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

	std::vector<Particle>& PsParticles() { return particles; }
	
	const std::vector<Particle>& ConstPsParticles() { return particles; }

	Particle& PsParticle(int i) { return particles[i]; }

	void SetNewDestination(int i, const glm::vec3& newDest);


private:

	int amount = 1000;

	std::vector<Particle> particles;

	Buffer particleBuffer;
	Buffer uniformBuffer;
	Texture texture;

	glm::vec3 pos;

	const glm::mat4* view = nullptr;
	glm::mat4* perspective = nullptr;

	UniformBufferParticle ubp;

};

