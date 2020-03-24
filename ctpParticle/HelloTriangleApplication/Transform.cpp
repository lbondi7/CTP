#include "Transform.h"

bool Transform::operator==(const Transform& other) const
{
	return pos == other.pos && scale == other.scale && rot == other.rot;
}

void Transform::operator=(const Transform& other)
{
	this->pos = other.pos;
	this->scale = other.scale;
	this->rot = other.rot;
}
