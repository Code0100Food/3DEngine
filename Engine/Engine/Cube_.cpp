#include "Cube_.h"

#include "MathGeoLib/Geometry/AABB.h"

// Constructors =================================
Cube_::Cube_()
{
	geometry = new math::AABB();
}

Cube_::Cube_(const Cube_ & _cpy)
{
	geometry = new math::AABB(*_cpy.geometry);
}

// Destructors ==================================
Cube_::~Cube_()
{
	RELEASE(geometry);
}

// Set Methods ==================================
void Cube_::SetMinPoint(math::float3 minp)
{
	geometry->minPoint = minp;
}

void Cube_::SetMaxPoint(math::float3 maxp)
{
	geometry->maxPoint = maxp;
}
