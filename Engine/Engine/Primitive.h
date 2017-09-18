#ifndef _PRIMITIVE_H_
#define _PRIMITIVE_H_

#include "glmath.h"
#include "Color.h"

enum PrimitiveTypes
{
	Primitive_Point,
	Primitive_Line,
	Primitive_Plane,
	Primitive_Cube,
	Primitive_Sphere,
	Primitive_Cylinder
};

enum AXIS {

	X,
	Y,
	Z,
	NEG_X,
	NEG_Y,
	NEG_Z
};

class _Primitive
{
public:

	_Primitive();
	_Primitive(float _posX, float _posY, float _posZ);

public:

	Color	color;
	mat4x4	transform;
	vec3	rotations;
	bool	axis, wire;

protected:

	PrimitiveTypes type;

public:

	virtual void	Render() const;
	virtual void	InnerRender() const;

	//Map building functions ------------------------------------------------
	void			SetPos(float x, float y, float z);
	void			SetPosFrom(_Primitive* origin, float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			SetMultiRotation(float x, float y, float z);
	void			AddAdjacentBody(_Primitive* target, float angle, AXIS, float x = 0, float y = 0, float z = 0);
	void			Scale(float x, float y, float z);
	void			SetColor(const Color& color);
	PrimitiveTypes	GetType() const;
	void			FadeColor(float r, float g, float b);

};

// ============================================
class Cube : public _Primitive
{
public:

	Cube();
	Cube(const Cube* _copy);
	Cube(float _sizeX, float _sizeY, float _sizeZ);
	Cube(float _sizeX, float _sizeY, float _sizeZ, float _posX, float _posY, float _posZ);

public:

	vec3 size;

public:

	void	InnerRender() const;
	void	ReSize(float x, float y, float z);

};

// ============================================
class _Sphere : public _Primitive
{
public:

	_Sphere();
	_Sphere(float _radius);
	_Sphere(float _radius, float _posX, float _posY, float _posZ);

public:

	float radius;

public:

	void InnerRender() const;

};

// ============================================
class _Cylinder : public _Primitive
{
public:

	_Cylinder();
	_Cylinder(float _radius, float _height);
	_Cylinder(float _radius, float _height, float _posX, float _posY, float _posZ);

public:

	float radius = 0.0f;
	float height = 0.0f;

public:

	void InnerRender() const;

};

// ============================================
class _Line : public _Primitive
{
public:

	_Line();
	_Line(float x, float _y, float _z);

public:

	vec3 origin;
	vec3 destination;

public:

	void InnerRender() const;

};

// ============================================
class _Plane : public _Primitive
{
public:

	_Plane();
	_Plane(float _x, float _y, float _z, float _d);

public:

	vec3	normal;
	float	constant = 0.0;

public:

	void InnerRender() const;

};

#endif // !_PRIMITIVE_H_


