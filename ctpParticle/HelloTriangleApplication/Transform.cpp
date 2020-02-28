#include "Transform.h"

bool TransformData::operator==(const TransformData& other) const
{
	return pos == other.pos && scale == other.scale && rot == other.rot;
}

void TransformData::operator=(const TransformData& other)
{
	pos = other.pos;
	scale = other.scale;
	rot = other.rot;
}
