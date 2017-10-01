#ifndef _FRUSTRUM_H_
#define _FRUSTRUM_H_

#include "Primitive_.h"

class Frustrum_ : public Primitive_
{
public:

	Frustrum_();
	Frustrum_(const Frustrum_& cpy);
	~Frustrum_();

public:

	void Initialize();
	void Draw();

public:
	
	math::Frustum* geometry = nullptr;

};



#endif // !_FRUSTRUM_H_
