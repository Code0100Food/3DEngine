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

public:

	bool Start();
	bool Update();

private:

	math::Frustum	frustum;
	bool			frustum_culling = false;

public:

	//Functionality ---------
	void SetTransform(const ComponentTransform* trans);
	void UpdateFrustrum();
	void DrawFrustum()const;

	void BlitComponentInspector();
	
};
#endif // !_COMPONENT_CAMERA_H_
