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

	math::Frustum frustum;

public:

	void SetTransform(const ComponentTransform* trans);
};
#endif // !_COMPONENT_CAMERA_H_
