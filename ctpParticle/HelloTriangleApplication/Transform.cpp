#include "Transform.h"

bool Transform::operator==(const Transform& other) const
{
	return position == other.position && scale == other.scale && rot == other.rot;
}

void Transform::operator=(const Transform& other)
{
	this->position = other.position;
	this->scale = other.scale;
	this->rot = other.rot;
}
