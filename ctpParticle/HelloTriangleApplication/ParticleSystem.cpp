#include "ParticleSystem.h"

#include "VkConstants.h"
#include "Locator.h"
#include "Timer.h"
#include "Keyboard.h"

#include <random>


ParticleSystem::ParticleSystem(int _amount, VkQueue graphicsQueue)
{
	//Create(_amount, graphicsQueue);
}

void ParticleSystem::Create(int _amount, VkQueue graphicsQueue)
{
	amount = _amount;

	//Create(graphicsQueue);
}

void ParticleSystem::Create(VkQueue graphicsQueue, const glm::mat4* _view, glm::mat4* _perspective)
{
	std::random_device rd;
	std::mt19937 mt;
	std::uniform_real_distribution<float> rand(-150.0f, 150.0f);
	//std::uniform_int_distribution<int> rand(-100, 100);
	std::uniform_real_distribution<float> rand2(1.0f, 10.0f);

	particles.resize(amount);

	for (size_t i = 0; i < amount; ++i)
	{
		//particles[i].position = { 100, 0.0f, 100 };
		particles[i].position = { rand(mt), rand(mt), rand(mt) };
		//particles[i].position = { 0, -5, 0 };
		//particles[i].position = { i - 2.0f, i + i + 2.0f, i + i + 2.0f };
		particles[i].velocity = { 0.0f, 0.0f, 0.0f };
		particles[i].maxLife = rand2(rd);
	}

	particleBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Particle) * amount);

	particleBuffer.StageBuffer(particleBuffer.size, graphicsQueue, particles.data());

	uniformBuffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBufferParticle));

	uniformBuffer.UpdateDescriptor(sizeof(UniformBufferParticle));

	texture.Load("particle2", graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);

	view = _view;
	perspective = _perspective;
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
	for (size_t i = 0; i < amount; ++i)
	{
		particles[i].position += particles[i].velocity * Locator::GetTimer()->DeltaTime();

		particles[i].alpha = 0.5f - ((particles[i].life / particles[i].maxLife) * 0.5f);

		//if (particles[i].life >= (particles[i].maxLife - 0.8f))
		//	particles[i].alpha = (particles[i].maxLife - particles[i].life);
		if (!lifeEnabled)
			continue;

		particles[i].life += Locator::GetTimer()->DeltaTime();
		if (particles[i].life >= particles[i].maxLife)
		{
			std::random_device rd;
			std::uniform_real_distribution<float> rand(-5.0f, 5.0f);
			std::uniform_real_distribution<float> rand2(1.0f, 10.0f);

			particles[i].position = { rand(rd), rand(rd), rand(rd) };

			particles[i].life = 0.0f;
			particles[i].maxLife = rand2(rd);
		}
	}

	ubp.model = glm::translate(glm::mat4(1.0f),  glm::vec3(0, 0, 0));
	ubp.view = *view;
	ubp.proj = *perspective;

	UpdateBuffers();
}

void ParticleSystem::UpdateBuffers()
{
	particleBuffer.CopyMem(particles.data(), particleBuffer.size);

	uniformBuffer.CopyMem(&ubp, sizeof(ubp));
}


void ParticleSystem::Destroy()
{
	particleBuffer.DestoryBuffer();
	uniformBuffer.DestoryBuffer();
	texture.Destroy();

	particles.clear();
}

void ParticleSystem::SetNewTarget(int i, const glm::vec3& newDest)
{
	particles[i].velocity = glm::normalize(newDest - particles[i].position) * 10.0f;
}
