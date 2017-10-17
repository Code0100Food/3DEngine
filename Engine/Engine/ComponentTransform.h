#ifndef _COMPONENT_TRANSFORM_H_
#define _COMPONENT_TRANSFORM_H_

#include "Component.h"
#include "MathGeoLib/Math/MathAll.h"

#include "Assimp/include/vector3.h"
#include "Assimp/include/quaternion.h"
#include "Assimp/include/matrix4x4.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform();
	ComponentTransform(const ComponentTransform& cpy);
	~ComponentTransform();

private:

	math::float4x4	transform_matrix;
	math::float3	position;
	math::float3	rotation_euler_angles;
	math::float3	scale;

public:

	//Set Methods -----------
	void SetTransformation(aiMatrix4x4 trans);
	void SetTransformation(math::float4x4 trans);

	//Get Methods -----------
	math::float3	GetPosition()const;

	//Functionality ---------
	void BlitComponentInspector();
};
#endif // !_COMPONENT_TRANSFORM_H_