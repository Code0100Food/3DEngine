#include "ComponentCamera.h"
#include "GameObject.h"
#include "ComponentTransform.h"

// Constructors =================================
ComponentCamera::ComponentCamera()
{
	
}

ComponentCamera::ComponentCamera(const ComponentCamera & cpy) :Component(cpy)
{

}

// Destructors ==================================
ComponentCamera::~ComponentCamera()
{

}

bool ComponentCamera::Start()
{
	SetFrustrum();

	return false;
}

// Functionality ================================
void ComponentCamera::SetTransform(const ComponentTransform * trans)
{

}

void ComponentCamera::SetFrustrum()
{
	//Set Planes distances
	frustum.farPlaneDistance = far_plane_distance;
	frustum.nearPlaneDistance = near_plane_distance;

	//Set frustum looking to Gameobject Z axis
	const Component* parent_transform = this->parent->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	

	
}
