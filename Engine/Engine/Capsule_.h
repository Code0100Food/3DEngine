#ifndef _CAPSULE_H_
#define _CAPSULE_H_

#include "Primitive_.h"

class Capsule;

class Capsule_ : public Primitive_ 
{
public:

	Capsule_();
	Capsule_(const Capsule_& _cpy);
	~Capsule_();

public:

	void Initialize();
	void Draw();

protected:

	math::Capsule* geometry = nullptr;

public:



};
#endif