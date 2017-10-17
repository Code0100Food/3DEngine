#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include "MathGeoLib/Geometry/Frustum.h"

class ComponentTransform;

class ComponentCamera :public  Component
{
public:

	ComponentCamera();
	ComponentCamera(const  ComponentCamera& cpy);
	~ComponentCamera();

private:

	math::Frustum	frustum;
	uint			far_plane_distance = 0;
	uint			near_plane_distance = 0;
	

public:

	bool Start();

	void SetTransform(const ComponentTransform* trans);
	void SetFrustrum();
};
#endif // !_COMPONENT_CAMERA_H_
