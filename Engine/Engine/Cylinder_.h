#ifndef _CYLINDER_H_
#define _CYLINDER_H_

#include "Primitive_.h"

class Cylinder;

class Cylinder_ : public Primitive_
{
public:

	Cylinder_();
	Cylinder_(const Cylinder_& _cpy);
	~Cylinder_();

public:

	void Initialize();
	void Draw();

public:

	math::Cylinder* geometry = nullptr;
	uint			vertex_num = 0;

};
#endif // !_CYLINDER_H_
