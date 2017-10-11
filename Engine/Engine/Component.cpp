#include "Component.h"

// Constructors =================================
Component::Component()
{

}

Component::Component(const Component & cpy)
{

}

// Destructors ==================================
Component::~Component()
{

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
