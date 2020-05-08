#include "ParticleSystem.h"

#include "VkConstants.h"
#include "Locator.h"
#include "Devices.h"
#include "Timer.h"
#include "Keyboard.h"
#include "Utillities.h"

#include <iostream>
#include <algorithm>

std::random_device rd;

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
	std::uniform_real_distribution<float> rand(-50.0f, 50.0f);
	std::uniform_real_distribution<float> rand2(1.0f, 10.0f);

	particles.resize(amount);

	for (size_t i = 0; i < amount; ++i)
	{
		particles[i].position = glm::vec4(Utillities::GetRandomVec3(glm::vec3(-100.0f), glm::vec3(100.0f)), 1.0f);
		particles[i].velocity = { 0.0f, 2.0f, 0.0f, 1.0f};
	}

	particleBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sizeof(Particle) * amount);

	Buffer stagingBuffer;
	stagingBuffer.CreateBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(Particle) * amount, particles.data());

	VkCommandBuffer copyCmd = Locator::GetDevices()->BeginSingleTimeCommands(VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
	VkBufferCopy copyRegion = {};
	copyRegion.size = particleBuffer.size;
	vkCmdCopyBuffer(copyCmd, stagingBuffer.buffer, particleBuffer.buffer, 1, &copyRegion);

	if (Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value() != 
		Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value())
	{
		VkBufferMemoryBarrier buffer_barrier =
		{
			VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
			nullptr,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			0,
            Locator::GetDevices()->GetQueueFamiliesIndices().graphicsFamily.value(),
            Locator::GetDevices()->GetQueueFamiliesIndices().computeFamily.value(),
			particleBuffer.buffer,
			0,
			particleBuffer.size
		};

		vkCmdPipelineBarrier(
			copyCmd,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
			VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
			0,
			0, nullptr,
			1, &buffer_barrier,
			0, nullptr);
	}
	Locator::GetDevices()->EndSingleTimeCommands(copyCmd, 1, graphicsQueue);

	particleBuffer.UpdateDescriptor(sizeof(Particle) * amount);

	stagingBuffer.DestoryBuffer();

	uniformBuffer.CreateBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, sizeof(UniformBuffer));

	uniformBuffer.UpdateDescriptor(sizeof(UniformBuffer));

	texture.Load("particle2", graphicsQueue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);


	view = _view;
	perspective = _perspective;
}

void ParticleSystem::Update()
{
	//for (size_t i = 0; i < amount; ++i)
	//{
	//	particles[i].position += particles[i].velocity * Locator::GetTimer()->FixedDeltaTime();
	//	
	//	//std::cout << particles[i].velocity.x << ", " << particles[i].velocity.y << ", " << particles[i].velocity.z << std::endl;

	//	//Utillities::Print(particles[i].velocity);

	//	particles[i].alpha = 0.5f - ((particles[i].life / particles[i].maxLife) * 0.5f);

	//	//if (particles[i].life >= (particles[i].maxLife - 0.8f))
	//	//	particles[i].alpha = (particles[i].maxLife - particles[i].life);
	//	if (!lifeEnabled)
	//		continue;

	//	particles[i].life += Locator::GetTimer()->FixedDeltaTime();
	//	if (particles[i].life >= particles[i].maxLife)
	//	{
	//		std::random_device rd;
	//		std::uniform_real_distribution<float> rand(-5.0f, 5.0f);
	//		std::uniform_real_distribution<float> rand2(1.0f, 10.0f);

	//		particles[i].position = { rand(rd), rand(rd), rand(rd) };

	//		particles[i].life = 0.0f;
	//		particles[i].maxLife = rand2(rd);
	//	}
	//}

	ubp.model = glm::translate(glm::mat4(1.0f),  glm::vec3(0, 0, 0));
	ubp.view = *view;
	ubp.proj = *perspective;

	UpdateBuffers();
}

void ParticleSystem::UpdateBuffers()
{
	//particleBuffer.CopyMem(particles.data(), particleBuffer.size);


	

	uniformBuffer.CopyMem(&ubp, sizeof(ubp));

	//computeParticleBuffer.CopyMem(particles.data(), computeParticleBuffer.size);
}


void ParticleSystem::Destroy()
{
	particleBuffer.DestoryBuffer();
	uniformBuffer.DestoryBuffer();
	texture.Destroy();
	particles.clear();
}

void ParticleSystem::SetParticleVelocityFromTarget(int i, const glm::vec3& newDest)
{
	//particles[i].target = newDest;
	//particles[i].velocity = glm::normalize(newDest.x - particles[i].position) * 1.0f;
}
