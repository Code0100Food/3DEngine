#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "Primitive_.h"

class AABB;

class Sphere_ : public Primitive_
{
public:

	Sphere_();
	Sphere_(const Sphere_& _cpy);
	~Sphere_();

public:

	void Initialize();
	void Draw();

protected:

	math::Sphere* geometry = nullptr;

public:

	//Set Methods -----------
	void SetPosition(math::float3 pos);
	void SetRad(float rad);

};
#endif