#pragma once

#include "glm/glm.hpp"

struct Transform
{
	glm::vec3 position = {0, 0, 0};
	glm::vec3 rot = { 0, 0, 0 };
	glm::vec3 scale = { 1, 1, 1 };


	bool operator==(const Transform& other) const;

	void operator=(const Transform& other);

};


//class TransformObject
//{
//public:
//
//	TransformObject() = default;
//
//	virtual ~TransformObject() = default;
//
//	const Transform& Transform() { return transform; }
//	void Transform(const Transform& t) { transform = t; }
//	void SetPos(const glm::vec3& pos) { transform.pos = pos; }
//	void SetScale(const glm::vec3& scale) { transform.scale = scale; }
//	void SetRot(const glm::vec3& rot) { transform.rot = rot; }
//
//
//protected:
//
//	const Transform& PrevTransform() { return prevTransform; }
//
//	void PrevTransform(const Transform& t) { prevTransform = t; }
//
//private:
//	Transform transform;
//	Transform prevTransform;
//};