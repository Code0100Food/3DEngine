#include "Component.h"

// Constructors =================================
Component::Component(COMPONENT_TYPE _type) :type(_type)
{

}

Component::Component(const Component & cpy) : type(cpy.type), parent(cpy.parent), actived(cpy.actived)
{

}

// Destructors ==================================
Component::~Component()
{
	parent = nullptr;
}

// Operators ====================================
bool Component::operator==(const Component & target)
{
	return bool(parent == target.parent &&type == target.type && actived == target.actived);
}

// Game Loop ====================================
bool Component::Update()
{
	return true;
}

// Set Methods ==================================
void Component::SetActiveState(bool act)
{
	actived = act;
}

void Component::SetParent(const GameObject * target)
{
	parent = (GameObject*)target;
}

COMPONENT_TYPE Component::GetType() const
{
	return type;
}
