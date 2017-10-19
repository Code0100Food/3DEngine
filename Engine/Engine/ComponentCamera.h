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
	void UpdateFrustumTransform();
	void DrawFrustum()const;
	void ApplyFrustum(GameObject* target);
	void UnApplyFrustum(GameObject* target);

	void BlitComponentInspector();
	
};
#endif // !_COMPONENT_CAMERA_H_
