#ifndef _BASIC_PRIMITIVE_H_
#define _BASIC_PRIMITIVE_H_

#include "Globals.h"
#include "Color.h"
#include "MathGeoLib/MathGeoLib.h"

enum PRIMITIVE_TYPE
{
	UNDEF_PRIMITIVE = 0,
	PRIMITIVE_POINT,
	PRIMITIVE_LINE,
	PRIMITIVE_PLANE,
	PRIMITIVE_CUBE,
	PRIMITIVE_SPHERE,
	PRIMITIVE_CYLINDER,
	PRIMITIVE_RAY,
	PRIMITIVE_CAPSULE,
	PRIMITIVE_FRUSTUM,
	PRIMITIVE_MESH
};

struct MeshData
{
	~MeshData()
	{
		delete []indices;
		delete []vertices;
	}

	uint	id_vertices = 0; // id in VRAM
	uint	num_indices = 0;
	uint*	indices = nullptr;

	uint	id_indices = 0; // id in VRAM
	uint	num_vertices = 0;
	float*	vertices = nullptr;
};

class Primitive_
{

	friend class GeometryManager;

public:

	Primitive_(PRIMITIVE_TYPE _type);
	Primitive_(const Primitive_& _cpy);
	virtual ~Primitive_();

public:

	virtual void	Initialize();
	void			Draw();

protected:

	Color			color = { 255,255,255,255 };
	bool			axis = false;
	PRIMITIVE_TYPE	type = UNDEF_PRIMITIVE;
	uint			divisions = 0;
	MeshData		mesh;

public:

	//Set Methods -----------
	void SetColor(const Color& n_color);
	void SetAxis(bool n_axis);
	void SetType(PRIMITIVE_TYPE n_type);
	void SetDivisions(uint def);

	//Get Methods -----------
	Color			GetColor()const;
	bool			GetAxis()const;
	PRIMITIVE_TYPE	GetType()const;

};

bool VertexToIndex(math::float3* all_vertex, uint vertex_num, std::vector<uint>* index, std::vector<math::float3>* vertex);

#endif // !_BASIC_PRIMITIVE_H_
