#pragma once

#include "glm/glm.hpp"

struct TransformData
{
	glm::vec3 pos = {0, 0, 0};
	glm::vec3 rot = { 0, 0, 0 };
	glm::vec3 scale = { 1, 1, 1 };


	bool operator==(const TransformData& other) const;

	void operator=(const TransformData& other);

};


class TransformObject
{
public:

	TransformObject() = default;

	virtual ~TransformObject() = default;

	const TransformData& Transform() { return transform; }
	void Transform(const TransformData& t) { transform = t; }
	void SetPos(const glm::vec3& pos) { transform.pos = pos; }
	void SetScale(const glm::vec3& scale) { transform.scale = scale; }
	void SetRot(const glm::vec3& rot) { transform.rot = rot; }


protected:

	const TransformData& PrevTransform() { return prevTransform; }

	void PrevTransform(const TransformData& t) { prevTransform = t; }

private:
	TransformData transform;
	TransformData prevTransform;
};