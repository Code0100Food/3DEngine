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
	PRIMITIVE_FRUSTUM
};

class Primitive_
{
public:

	Primitive_(PRIMITIVE_TYPE _type);
	Primitive_(const Primitive_& _cpy);
	virtual ~Primitive_();

public:

	virtual void Initialize() = 0;
	virtual void Draw() = 0;

protected:

	Color			color = { 255,255,255,255 };
	bool			axis = false;
	PRIMITIVE_TYPE	type = UNDEF_PRIMITIVE;
	uint			definition = 0;
	uint			vertex_buffer_id = 0;
	uint			index_buffer_id = 0;

public:

	//Set Methods -----------
	void SetColor(const Color& n_color);
	void SetAxis(bool n_axis);
	void SetType(PRIMITIVE_TYPE n_type);
	void SetDefinition(uint def);

	//Get Methods -----------
	Color			GetColor()const;
	bool			GetAxis()const;
	PRIMITIVE_TYPE	GetType()const;

};
#endif // !_BASIC_PRIMITIVE_H_
