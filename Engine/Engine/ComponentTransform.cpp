#include "ComponentTransform.h"

// Constructors =================================
ComponentTransform::ComponentTransform() : Component(COMP_TRANSFORMATION)
{

}

ComponentTransform::ComponentTransform(const ComponentTransform & cpy) : Component(cpy), position(cpy.position), scale(cpy.scale), rotation(cpy.rotation)
{

}

// Destructors ==================================
ComponentTransform::~ComponentTransform()
{

}

// Set Methods ==================================
void ComponentTransform::SetTransformation(aiMatrix4x4 trans)
{
	trans.Decompose(scale, rotation, position);
}
