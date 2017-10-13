#ifndef _COMPONENT_TRANSFORM_H_
#define _COMPONENT_TRANSFORM_H_

#include "Component.h"

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

	aiVector3D		position;
	aiVector3D		scale;
	aiQuaternion	rotation;

public:

	//Set Methods -----------
	void SetTransformation(aiMatrix4x4 trans);

	//Get Methods -----------
	aiVector3D	GetPosition()const;

	//Functionality ---------
	void BlitComponentInspector();
};
#endif // !_COMPONENT_TRANSFORM_H_
