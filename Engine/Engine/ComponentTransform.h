#ifndef _COMPONENT_TRANSFORM_H_
#define _COMPONENT_TRANSFORM_H_

#include "Component.h"
#include "MathGeoLib/Math/MathAll.h"
#include "Assimp/include/matrix4x4.h"

class ComponentTransform : public Component
{
public:

	ComponentTransform();
	ComponentTransform(const ComponentTransform& cpy);
	~ComponentTransform();

public:

	bool Update(float dt);

private:

	//Local transform
	math::float4x4	transform_matrix = math::float4x4::identity;
	math::float3	position = { 0,0,0 };

	//Inherited transform from GameObject parents
	math::float4x4	inherited_transform = math::float4x4::identity;

	math::float3	scale = { 1,1,1 };

	//Rotation for user
	math::float3	rotation_euler_angles = { 0,0,0 };

	//Rotation for computer
	math::Quat		rotation_quaternion = math::Quat::identity;

	//Draw axis
	bool draw_axis = true;

public:

	bool has_been_modified = false;

public:

	//Set Methods -----------
	void SetTransformation(aiMatrix4x4 trans);
	void SetTransformation(math::float4x4 trans);

	//Get Methods -----------
	math::float3	GetPosition()const;
	math::float3	GetRotationEuler()const;
	math::Quat		GetRotationQuat()const;
	math::float3	GetScale()const;

	math::float4x4	GetTransform() const;
	math::float4x4	GetInheritedTransform() const;
	const float*	GetTransformMatrixRows() const;
	const float*	GetTransformMatrixColumns() const;

	//Functionality ---------
	void BlitComponentInspector();

	void UpdateTransform();

	void SetMatrixToDraw();
	void QuitMatrixToDraw();

	void DrawOrientationAxis();
	void UpdateRotationPositionScale();

	//Save/Load Methods -----
	bool Save(Serializer& array_root)const;
	bool Load(Serializer& data, std::vector<std::pair<Component*, uint>>& links);

};

#endif // !_COMPONENT_TRANSFORM_H_