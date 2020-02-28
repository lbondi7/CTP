#pragma once

#include "Buffer.h"
#include "Texture.h"
#include "VkHelper.h"

#include "glm//glm.hpp"
#include <vector>

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity;

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

class ParticleSystem
{
public:

	ParticleSystem() = default;
	ParticleSystem(int _amount, VkQueue graphicsQueue);

	void Create(int _amount, VkQueue graphicsQueue);

	void Create(VkQueue graphicsQueue);

	void Update();

	void Destroy();


	int ParticleCount() { return amount; }

	Texture& PsTexture() { return texture; }

	Buffer& PBuffer() { return particleBuffer; }

	Buffer& UBuffer() { return uniformBuffer; }


	std::vector<Particle>& PsParticles(int i) { return particles; }
	
	const std::vector<Particle>& ConstPsParticles(int i) { return particles; }

	Particle& PsParticle(int i) { return particles[i]; }

private:

	int amount = 2000000;

	std::vector<Particle> particles;

	Buffer particleBuffer;
	Buffer uniformBuffer;
	Texture texture;
};

