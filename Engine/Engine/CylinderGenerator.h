#ifndef _CYLINDER_GENERATOR_
#define _CYLINDER_GENERATOR_

#include "PrimitiveGenerator.h"
#include "MathGeoLib/Geometry/Cylinder.h"

class CylinderGenerator : public PrimitiveGenerator
{
public:

	CylinderGenerator();
	~CylinderGenerator();

public:

	std::pair<std::vector<uint>, std::vector<Vertex>> Generate()final;

private:

	math::Cylinder	geometry;

public:

	//Set Methods -----------
	void SetGeometry(const math::Cylinder& geom);
	void SetTop(math::float3 top);
	void SetBottom(math::float3 bottom);
	void SetRad(uint rad);

};
#endif // !_CYLINDER_GENERATOR_
