#ifndef _LIGHT_H_
#define _LIGHT_H_

#include "Color.h"
#include "MathGeoLib/Math/float3.h"

struct Light
{
	Light();

	void Init();
	void SetPos(float x, float y, float z);
	void Active(bool active);
	void Render();

	Color			ambient;
	Color			diffuse;
	math::float3	position;

	int ref = 0;
	bool on = false;
};


#endif // !_LIGHT_H_


