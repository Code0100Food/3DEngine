#ifndef _COMPONENT_CAMERA_H_
#define _COMPONENT_CAMERA_H_

#include "Component.h"
#include <queue>
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
	bool Update(float dt);

private:

	math::Frustum	frustum;
	bool			frustum_culling = false;
	bool			is_main = false;

	//Queue to apply frustum culling to all childs
	std::queue<GameObject*>	remaining_childs;

public:

	//Get Methods -----------
	const math::Frustum GetFrustum() const;

	//Set Methods -----------
	void SetIsMain(bool value);
	void SetVerticalFov(float angle_in_deg, float aspect_ratio);

	//Functionality ---------
	void SetTransform(const ComponentTransform* trans);
	
	//Frustum Methods -------
	void UpdateFrustumTransform();
	void DrawFrustum()const;
	void ApplyDinamicFrustum();
	void ApplyStaticFrustum();
	void UnApplyFrustum(GameObject* target);

	void BlitComponentInspector();
	
	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);
};
#endif // !_COMPONENT_CAMERA_H_
