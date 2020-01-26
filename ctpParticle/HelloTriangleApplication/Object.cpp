#include "Object.h"


void Object::Init(const char* _model, const char* _texture, VkQueue queue)
{

	model.Load(_model, queue);

	texture.Load(_texture, queue, VK_FORMAT_R8G8B8A8_UNORM,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);


}

Model& Object::GetModel()
{
	return model;
}

Texture& Object::GetTexture()
{
	return texture;
}
