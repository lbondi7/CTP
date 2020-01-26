#pragma once

#include "Model.h"
#include "Texture.h"

class Object
{
public:
	Object() = default;
	~Object() = default;

	void Init(std::string _model, std::string _texture, VkQueue queue);

	void Init(const std::string& _model, std::string _texture, VkQueue queue);

	void Init(const char* _model, const char* _texture, VkQueue queue);

	Model& GetModel();
	Texture& GetTexture();

private:

	Model model;
	Texture texture;

};

