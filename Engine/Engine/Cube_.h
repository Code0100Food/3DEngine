#ifndef _CUBE__H_
#define _CUBE__H_

#include "Primitive_.h"
#include "Globals.h"

class AABB;

class Cube_ : public Primitive_
{
public:

	Cube_();
	Cube_(const Cube_& cpy);
	~Cube_();

protected:
	
	math::AABB* geometry = nullptr;

public:

	//SetMethods ------------
	void SetMinPoint(math::float3 minp);
	void SetMaxPoint(math::float3 maxp);

};

#endif // !_CUBE__H_
