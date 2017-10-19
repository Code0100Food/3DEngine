#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Globals.h"
#include "MathGeoLib/Geometry/Sphere.h"
#include "ComponentMesh.h"

class Sphere_
{
public:

	Sphere_();
	Sphere_(const Sphere_& _cpy);
	~Sphere_();

public:

	std::pair<std::vector<uint>, std::vector<Vertex>> Initialize();

protected:

	math::Sphere	geometry;
	uint			divisions = 0;

public:

	//Set Methods -----------
	void SetPosition(math::float3 pos);
	void SetRad(float rad);
	void SetDivisions(uint div);

};
#endif