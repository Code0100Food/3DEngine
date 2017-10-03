#ifndef _MESH_H_
#define _MESH_H_

#include "Primitive_.h"
#include "Globals.h"
#include "MathGeoLib/MathGeoLib.h"

struct Vertex 
{
	math::float3 position = { 0,0,0 };
	math::float3 normal = { 0,0,0 };
	math::float2 tex_coords = { 0,0 };
};

struct Texture 
{
	uint		id = 0;
	std::string	type;
};

class Mesh_ : public Primitive_
{
public:

	Mesh_();
	Mesh_(const Mesh_& _cpy);
	~Mesh_();

protected:

	/*Mesh Data*/

};
#endif // !_MESH_H_
