
#pragma once
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

public:
	
	Color color;
	mat4x4 transform;
	vec3 rotations;
	bool axis,wire;

	void	FadeColor(float r, float g, float b);

protected:

	PrimitiveTypes type;
};

// ============================================
class Cube : public Primitive
{
public :
	Cube();
	Cube(const Cube* copy);
	Cube(float sizeX, float sizeY, float sizeZ);
	Cube(float sizeX, float sizeY, float sizeZ, float posX, float posY, float posZ);

	void InnerRender() const;

	void	ReSize(float x, float y, float z);

public:
	vec3 size;
};

// ============================================
class Sphere : public Primitive
{
public:
	Sphere();
	Sphere(float radius);
	Sphere(float radius, float posX, float posY, float posZ);
	void InnerRender() const;
public:
	float radius;
};

// ============================================
class Cylinder : public Primitive
{
public:
	Cylinder();
	Cylinder(float radius, float height);
	Cylinder(float radius, float height, float posX, float posY, float posZ);
	void InnerRender() const;
public:
	float radius;
	float height;
};

// ============================================
class Line : public Primitive
{
public:
	Line();
	Line(float x, float y, float z);
	void InnerRender() const;
public:
	vec3 origin;
	vec3 destination;
};

// ============================================
class Plane : public Primitive
{
public:
	Plane();
	Plane(float x, float y, float z, float d);
	void InnerRender() const;
public:
	vec3 normal;
	float constant;
};