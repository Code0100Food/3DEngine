#include "Application.h"
#include "Globals.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "Primitive.h"
#include "glut/glut.h"


#pragma comment (lib, "Engine/glut/glut32.lib")

// ------------------------------------------------------------
_Primitive::_Primitive() : transform(IdentityMatrix), color(White), wire(false), axis(false), rotations({0,0,0}),type(PrimitiveTypes::Primitive_Point)
{}

_Primitive::_Primitive(float _posX, float _posY, float _posZ) : transform(IdentityMatrix), color(White), wire(false), axis(false), rotations({ 0,0,0 }), type(PrimitiveTypes::Primitive_Point)
{
	transform.translate(_posX, _posY, _posZ);
}

// ------------------------------------------------------------
PrimitiveTypes _Primitive::GetType() const
{
	return type;
}

void _Primitive::FadeColor(float r, float g, float b)
{
	color.r += r;
	color.g += g;
	color.b += b;
}

// ------------------------------------------------------------
void _Primitive::Render() const
{
	glPushMatrix();
	glMultMatrixf(transform.M);

	if(axis == true)
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	glColor3f(color.r, color.g, color.b);

	if(wire)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	InnerRender();

	glPopMatrix();
}

// ------------------------------------------------------------
void _Primitive::InnerRender() const
{
	glPointSize(5.0f);

	glBegin(GL_POINTS);

	glVertex3f(0.0f, 0.0f, 0.0f);

	glEnd();

	glPointSize(1.0f);
}

// ------------------------------------------------------------
void _Primitive::SetPos(float x, float y, float z)
{
	transform.translate(x, y, z);
}

void _Primitive::SetPosFrom(_Primitive * origin, float x, float y, float z)
{
	vec3 org = origin->transform.translation();
	transform.translate(org.x + x, org.y + y, org.z + z);
}

// ------------------------------------------------------------
void _Primitive::SetRotation(float angle, const vec3 &u)
{
	transform.rotate(angle, u);

}

void _Primitive::SetMultiRotation(float x, float y, float z)
{

	mat4x4 rot_x = transform.rotate(x, { -1,0,0 });
	mat4x4 rot_y = transform.rotate(y, { 0,-1,0 });
	mat4x4 rot_z = transform.rotate(z, { 0,0,-1 });
	transform = rot_x* rot_y *rot_z;
}

void _Primitive::AddAdjacentBody(_Primitive * target, float angle, AXIS axis, float x, float y, float z)
{
	//Calculate the point of the parent object
	vec3 Apoint;

	/*
	AA		AB
	0.......0
	.		.
	.		.
	.		.
	0.......0
	BA		BB

	*/
	
	//Apoint = AB
	if (angle < -1 && axis == Y && this->rotations.y < 180) {
		Apoint.x = ((Cube*)this)->size.x * 0.5f;
		Apoint.y = 0.0f;
		Apoint.z = ((Cube*)this)->size.z * 0.5f;
	}
	//Apoint = AA
	else if(this->rotations.y < 180){
		Apoint.x = (((Cube*)this)->size.x * 0.5f);
		Apoint.y = 0.0f;
		Apoint.z = -(((Cube*)this)->size.z * 0.5f);
	}
	//Apoint = BA
	else if (angle < -1 && axis == Y && this->rotations.y >= 180) {
		Apoint.x = -((Cube*)this)->size.x * 0.5f;
		Apoint.y = 0.0f;
		Apoint.z = -((Cube*)this)->size.z * 0.5f;
	}
	//Apoint = BB
	else {
		Apoint.x = -(((Cube*)this)->size.x * 0.5f);
		Apoint.y = 0.0f;
		Apoint.z = (((Cube*)this)->size.z * 0.5f);
	}

	if(this->rotations.z < 180)Apoint = (rotate(Apoint, this->rotations.z, { 0,0,-1.0f }));
	else Apoint = (rotate(Apoint, this->rotations.z - 180, { 0,0,-1.0f }));
	
	if(this->rotations.y < 180)Apoint = (rotate(Apoint, this->rotations.y, { 0,-1.0f,0 }));
	else Apoint = (rotate(Apoint, this->rotations.y - 180, { 0,-1.0f,0 }));
	
	Apoint = (rotate(Apoint, this->rotations.x, { -1.0f,0,0 }));

	//Calculate the point in the child object(rotate parent y)
	vec3 Bpoint;
	//Bpoint from AB
	if (angle < -1 && axis == Y && this->rotations.y < 180) {
		Bpoint.x = Apoint.x + (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z - (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from AA
	else if(this->rotations.y < 180){
		Bpoint.x = Apoint.x + (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z + (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from BA
	else if (angle < -1 && axis == Y && this->rotations.y > 180) {
		Bpoint.x = Apoint.x - (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z + (((Cube*)target)->size.z * 0.5f) + z;
	}
	//Bpoint from BB
	else {
		Bpoint.x = Apoint.x - (((Cube*)target)->size.x * 0.5f) + x;
		Bpoint.y = y;
		Bpoint.z = Apoint.z - (((Cube*)target)->size.z * 0.5f) + z;
	}



	if (this->rotations.z != 0) { 
		
		Bpoint.y = Apoint.y + y;

	}

	//Vector form A to B
	vec3 vector(Bpoint.x - Apoint.x, Bpoint.y - Apoint.y, Bpoint.z - Apoint.z);

	//Rotate the entered angle
	switch (axis) {

	case AXIS::Y:
		
		if (this->rotations.z < 180)vector = rotate(vector, this->rotations.z, { 0,0,-1.0f });
		else vector = rotate(vector, this->rotations.z - 180, { 0,0,-1.0f });
		if (this->rotations.y < 180)vector = rotate(vector, this->rotations.y + angle, { 0,-1.0f,0 });
		else vector = rotate(vector, this->rotations.y - 180 + angle, { 0,-1.0f,0 });
		
		
		//Update the body angle
		target->rotations.x = this->rotations.x;
		target->rotations.y = angle + this->rotations.y;
		target->rotations.z = this->rotations.z;

		target->SetMultiRotation(this->rotations.x, angle + this->rotations.y, this->rotations.z);
		break;

	case AXIS::Z:
		
		if(this->rotations.z < 180)vector = rotate(vector, this->rotations.z + angle , { 0,0,-1.0f });
		else vector = rotate(vector, this->rotations.z - 180 + angle, { 0,0,-1.0f });
		if (this->rotations.y < 180)vector = rotate(vector, this->rotations.y, { 0,-1.0f,0 });
		else vector = rotate(vector, this->rotations.y - 180, { 0,-1.0f,0 });
		
		
		//Update the body angle
		target->rotations.x = this->rotations.x;
		target->rotations.y = this->rotations.y;
		target->rotations.z = angle + this->rotations.z;

		target->SetMultiRotation(this->rotations.x, this->rotations.y, angle + this->rotations.z);
		break;

	}
	//Set the data calculated
	target->SetPosFrom(this, Apoint.x + vector.x, Apoint.y + vector.y, Apoint.z + vector.z);
	
}

// ------------------------------------------------------------
void _Primitive::Scale(float x, float y, float z)
{
	transform.scale(x, y, z);
}

void _Primitive::SetColor(const Color & color)
{
	this->color = color;
}

// CUBE ============================================
Cube::Cube() : _Primitive(), size(1.0f, 1.0f, 1.0f)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(const Cube * _copy)
{
	this->transform = _copy->transform;
	this->type = _copy->type;
	this->axis = _copy->axis;
	this->color = _copy->color;
	this->rotations = _copy->rotations;
	this->size = _copy->size;
	this->wire = _copy->wire;
}

Cube::Cube(float _sizeX, float _sizeY, float _sizeZ) : _Primitive(), size(_sizeX, _sizeY, _sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

Cube::Cube(float _sizeX, float _sizeY, float _sizeZ, float _posX, float _posY, float _posZ) : _Primitive(_posX, _posY, _posZ), size(_sizeX, _sizeY, _sizeZ)
{
	type = PrimitiveTypes::Primitive_Cube;
}

void Cube::InnerRender() const
{	
	float sx = size.x * 0.5f;
	float sy = size.y * 0.5f;
	float sz = size.z * 0.5f;

	glBegin(GL_QUADS);

	glNormal3f(0.0f, 0.0f, 1.0f);
	glVertex3f(-sx, -sy, sz);
	glVertex3f( sx, -sy, sz);
	glVertex3f( sx,  sy, sz);
	glVertex3f(-sx,  sy, sz);

	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f( sx, -sy, -sz);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx,  sy, -sz);
	glVertex3f( sx,  sy, -sz);

	glNormal3f(1.0f, 0.0f, 0.0f);
	glVertex3f(sx, -sy,  sz);
	glVertex3f(sx, -sy, -sz);
	glVertex3f(sx,  sy, -sz);
	glVertex3f(sx,  sy,  sz);

	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f(-sx, -sy,  sz);
	glVertex3f(-sx,  sy,  sz);
	glVertex3f(-sx,  sy, -sz);

	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-sx, sy,  sz);
	glVertex3f( sx, sy,  sz);
	glVertex3f( sx, sy, -sz);
	glVertex3f(-sx, sy, -sz);

	glNormal3f(0.0f, -1.0f, 0.0f);
	glVertex3f(-sx, -sy, -sz);
	glVertex3f( sx, -sy, -sz);
	glVertex3f( sx, -sy,  sz);
	glVertex3f(-sx, -sy,  sz);

	glEnd();
}

void Cube::ReSize(float x, float y, float z)
{
	size.x = x;
	size.y = y;
	size.z = z;
}

// SPHERE ============================================
_Sphere::_Sphere() : _Primitive(), radius(1.0f)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

_Sphere::_Sphere(float _radius) : _Primitive(), radius(_radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

_Sphere::_Sphere(float _radius, float _posX, float _posY, float _posZ) : _Primitive(_posX, _posY, _posZ), radius(_radius)
{
	type = PrimitiveTypes::Primitive_Sphere;
}

void _Sphere::InnerRender() const
{
	glutSolidSphere(radius, 25, 25);
}


// CYLINDER ============================================
_Cylinder::_Cylinder() : _Primitive(), radius(1.0f), height(1.0f)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

_Cylinder::_Cylinder(float _radius, float _height) : _Primitive(), radius(_radius), height(_height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

_Cylinder::_Cylinder(float _radius, float _height, float _posX, float _posY, float _posZ) : _Primitive(_posX, _posY, _posZ), radius(_radius), height(_height)
{
	type = PrimitiveTypes::Primitive_Cylinder;
}

void _Cylinder::InnerRender() const
{
	int n = 30;

	// Cylinder Bottom
	glBegin(GL_POLYGON);
	
	for(int i = 360; i >= 0; i -= (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder Top
	glBegin(GL_POLYGON);
	glNormal3f(0.0f, 0.0f, 1.0f);
	for(int i = 0; i <= 360; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians
		glVertex3f(height * 0.5f, radius * cos(a), radius * sin(a));
	}
	glEnd();

	// Cylinder "Cover"
	glBegin(GL_QUAD_STRIP);
	for(int i = 0; i < 480; i += (360 / n))
	{
		float a = i * M_PI / 180; // degrees to radians

		glVertex3f(height*0.5f,  radius * cos(a), radius * sin(a) );
		glVertex3f(-height*0.5f, radius * cos(a), radius * sin(a) );
	}
	glEnd();
}

// LINE ==================================================
_Line::_Line() : _Primitive(), origin(0, 0, 0), destination(1, 1, 1)
{
	type = PrimitiveTypes::Primitive_Line;
}

_Line::_Line(float _x, float _y, float _z) : _Primitive(), origin(0, 0, 0), destination(_x, _y, _z)
{
	type = PrimitiveTypes::Primitive_Line;
}

void _Line::InnerRender() const
{
	glLineWidth(2.0f);

	glBegin(GL_LINES);

	glVertex3f(origin.x, origin.y, origin.z);
	glVertex3f(destination.x, destination.y, destination.z);

	glEnd();

	glLineWidth(1.0f);
}

// PLANE ==================================================
_Plane::_Plane() : _Primitive(), normal(0, 1, 0), constant(1)
{
	type = PrimitiveTypes::Primitive_Plane;
}

_Plane::_Plane(float _x, float _y, float _z, float _d) : _Primitive(), normal(_x, _y, _z), constant(_d)
{
	type = PrimitiveTypes::Primitive_Plane;
}

void _Plane::InnerRender() const
{
	glLineWidth(1.0f);

	glBegin(GL_LINES);

	float d = 200.0f;

	for(float i = -d; i <= d; i += 1.0f)
	{
		glVertex3f(i, 0.0f, -d);
		glVertex3f(i, 0.0f, d);
		glVertex3f(-d, 0.0f, i);
		glVertex3f(d, 0.0f, i);
	}

	glEnd();
}