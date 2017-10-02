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

	void Initialize()final;

public:

	math::Cylinder* geometry = nullptr;

};
#endif // !_CYLINDER_H_
