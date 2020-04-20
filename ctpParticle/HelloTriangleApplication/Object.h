#pragma once

#include "Model.h"
#include "Texture.h"


class Object
{
public:
	
	virtual ~Object() = default;

	const Transform& GetTransform() { return transform; }
	void SetTransform(const Transform& t) { transform = t; }
	void SetPos(const glm::vec3& pos) { transform.position = pos; }
	void SetScale(const glm::vec3& scale) { transform.scale = scale; }
	void SetRot(const glm::vec3& rot) { transform.rot = rot; }

protected:

	Transform transform;

};

class GameObject : public Object
{
public:
	GameObject() = default;
	~GameObject() = default;

	void Init(const char* _model, const char* _texture, VkQueue queue);

	void Update();

	Model& GetModel();
	Texture& GetTexture();

	void Destroy();

private:

	Model model;
	Texture texture;

};

