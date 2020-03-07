#include "Object.h"


void GameObject::Init(const char* _model, const char* _texture, VkQueue queue)
{
	model.Load(_model, queue);

	texture.Load(_texture, queue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
}

void GameObject::Update()
{
	model.Update();
}

Model& GameObject::GetModel()
{
	return model;
}

Texture& GameObject::GetTexture()
{
	return texture;
}

void GameObject::Destroy()
{
	model.Destroy();
	texture.Destroy();
}