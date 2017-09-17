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

class Primitive
{
public:

	Primitive();
	Primitive(float posX, float posY, float posZ);

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
	void			SetPosFrom(Primitive* origin, float x, float y, float z);
	void			SetRotation(float angle, const vec3 &u);
	void			SetMultiRotation(float x, float y, float z);
	void			AddAdjacentBody(Primitive* target, float angle, AXIS, float x = 0, float y = 0, float z = 0);
	void			Scale(float x, float y, float z);
	void			SetColor(const Color& color);
	PrimitiveTypes	GetType() const;
	void			FadeColor(float r, float g, float b);

};

// ============================================
class Cube : public Primitive
{
public:

	Cube();
	Cube(const Cube* copy);
	Cube(float sizeX, float sizeY, float sizeZ);
	Cube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

public:

	vec3 size;

public:

	void	InnerRender() const;
	void	ReSize(float x, float y, float z);

};

// ============================================
class Sphere : public Primitive
{
public:

	Sphere();
	Sphere(float radius);
	Sphere(float radius, float posX, float posY, float posZ);

public:

	float radius;

public:

	void InnerRender() const;

};

// ============================================
class Cylinder : public Primitive
{
public:

	Cylinder();
	Cylinder(float radius, float height);
	Cylinder(float radius, float height, float posX, float posY, float posZ);

public:

	float radius = 0.0f;
	float height = 0.0f;

public:

	void InnerRender() const;

};

// ============================================
class Line : public Primitive
{
public:

	Line();
	Line(float x, float y, float z);

public:

	vec3 origin;
	vec3 destination;

public:

	void InnerRender() const;

};

// ============================================
class Plane : public Primitive
{
public:

	Plane();
	Plane(float x, float y, float z, float d);

public:

	vec3	normal;
	float	constant = 0.0;

public:

	void InnerRender() const;

};

#endif // !_PRIMITIVE_H_


