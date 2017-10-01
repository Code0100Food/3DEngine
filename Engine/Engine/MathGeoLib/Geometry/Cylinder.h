#pragma once

#include "../Math/float3.h"
#include "LineSegment.h"
#include <vector>

MATH_BEGIN_NAMESPACE

class Cylinder
{
public:

	Cylinder();
	Cylinder(const Cylinder& _cpy);
	~Cylinder();

public:

	unsigned int r = 0;
	math::LineSegment l;

public:

	std::vector<math::float3> Triangulate(int slices) const;
	
};

MATH_END_NAMESPACE