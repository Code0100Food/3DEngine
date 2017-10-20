#ifndef _CUBE_GENERATOR_
#define _CUBE_GENERATOR_

#include "PrimitiveGenerator.h"
#include "MathGeoLib/Geometry/AABB.h"

class CubeGenerator : public PrimitiveGenerator
{
public:

	CubeGenerator();
	~CubeGenerator();

public:

	std::pair<std::vector<uint>, std::vector<Vertex>> Generate()final;

private:

	math::AABB geometry;

public:

	//Set Methods -----------
	void SetMinPoint(math::float3 minp);
	void SetMaxPoint(math::float3 maxp);

};
#endif // !_CUBE_GENERATOR_
