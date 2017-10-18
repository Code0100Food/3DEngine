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
	math::float3	scale;

	//Rotation for user
	math::float3	rotation_euler_angles;

	//Rotation for computer
	math::Quat		rotation_quaternion;

public:

	bool Update();

	//Set Methods -----------
	void SetTransformation(aiMatrix4x4 trans);
	void SetTransformation(math::float4x4 trans);

	//Get Methods -----------
	math::float3	GetPosition()const;
	math::float3	GetRotationEuler()const;
	math::Quat		GetRotationQuat()const;
	math::float3	GetScale()const;

	const float*	GetTransformMatrixRows() const;
	const float*	GetTransformMatrixColumns() const;

	//Functionality ---------
	void BlitComponentInspector();

	bool has_been_modified = false;
	void UpdateTransform();
};
#endif // !_COMPONENT_TRANSFORM_H_