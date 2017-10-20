#ifndef _SPHERE_GENERATOR_H_
#define _SPHERE_GENERATOR_H_

#include "PrimitiveGenerator.h"
#include "MathGeoLib/Geometry/Sphere.h"

class SphereGenerator : public PrimitiveGenerator
{
public:

	SphereGenerator();
	~SphereGenerator();

public:

	std::pair<std::vector<uint>, std::vector<Vertex>> Generate()final;

private:

	math::Sphere	geometry;

public:

	//Set Methods -----------
	void SetPosition(math::float3 pos);
	void SetRad(float rad);

};
#endif