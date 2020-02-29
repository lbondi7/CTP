#include "Transform.h"

bool Transform::operator==(const Transform& other) const
{
	return pos == other.pos && scale == other.scale && rot == other.rot;
}

void Transform::operator=(const Transform& other)
{
	pos = other.pos;
	scale = other.scale;
	rot = other.rot;
}
