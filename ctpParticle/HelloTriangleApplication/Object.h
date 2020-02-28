#pragma once

#include "Model.h"
#include "Texture.h"

class Object : public TransformObject
{
public:
	Object() = default;
	~Object() = default;

	void Init(const char* _model, const char* _texture, VkQueue queue);

	Model& GetModel();
	Texture& GetTexture();

	void Destroy();

private:

	Model model;
	Texture texture;

};

