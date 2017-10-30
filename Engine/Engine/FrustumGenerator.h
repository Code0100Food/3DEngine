#ifndef _FRUSTUM_GENERATOR_H_
#define _FRUSTUM_GENERATOR_H_

#include "PrimitiveGenerator.h"
#include "MathGeoLib/Geometry/Frustum.h"

class FrustumGenerator : public PrimitiveGenerator
{
public:

	FrustumGenerator();
	~FrustumGenerator();

public:

	std::pair<std::vector<uint>, std::vector<Vertex>> Generate()final;

private:

	math::Frustum geometry;

public:

	//Set Methods -----------
	//void SetMinPoint(math::float3 minp);
	//void SetMaxPoint(math::float3 maxp);

};
#endif // !_FRUSTUM_GENERATOR_H_
