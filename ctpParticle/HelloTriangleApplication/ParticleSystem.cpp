#include "ParticleSystem.h"

#include "VkConstants.h"
#include "Locator.h"
#include "Timer.h"

#include <random>

ParticleSystem::ParticleSystem(int _amount, VkQueue graphicsQueue)
{
	Create(_amount, graphicsQueue);

}

void ParticleSystem::Create(int _amount, VkQueue graphicsQueue)
{
	amount = _amount;

	Create(graphicsQueue);
}

void ParticleSystem::Create(VkQueue graphicsQueue)
{
	std::random_device rd;
	std::uniform_real_distribution<float> rand(-1.0f, 1.0f);

	particles.resize(amount);

	for (size_t i = 0; i < amount; i++)
	{
		particles[i].position = { rand(rd) * 300.0f, rand(rd) * 20.0f, rand(rd) * 100.0f };
		particles[i].velocity = { 0.0f, 0.0f, 0.0f };
	}

	particleBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Particle) * amount);

	particleBuffer.StageBuffer(particleBuffer.size, graphicsQueue, particles.data());

	uniformBuffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferParticle));

	uniformBuffer.UpdateDescriptor(sizeof(UniformBufferParticle));

	texture.Load("particle2", graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
}

glm::vec3 getFlowField(glm::vec3 pos)
{
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, -pos.x * pos.y) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z)));
	glm::vec3 vel = glm::vec3(-pos.y, pos.x, pos.x) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y) + (pos.z * pos.z));
	//glm::vec3 vel = (glm::vec3(-pos.y, pos.x, 0) / std::sqrt((pos.x * pos.x) + (pos.y * pos.y)));
	//vel.x = std::sqrt((pos.x * pos.x) + (pos.y * pos.y));
	//vel.y = 0;
	//vel.z = 0;

	return vel;
}

void ParticleSystem::Update()
{

	for (size_t i = 0; i < amount; i++)
	{
		particles[i].position += getFlowField(particles[i].position) * Locator::GetTimer()->DeltaTime() * 100.0f;
	}

	particleBuffer.CopyMem(particles.data(), particleBuffer.size);
}

void ParticleSystem::Destroy()
{
	particleBuffer.DestoryBuffer();
	uniformBuffer.DestoryBuffer();
	texture.Destroy();

	particles.clear();
}
