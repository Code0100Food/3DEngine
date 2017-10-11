#include "GameObject.h"
#include "Component.h"

// Constructors =================================
GameObject::GameObject()
{

}

GameObject::GameObject(const GameObject & cpy)
{

}

// Destructors ==================================
GameObject::~GameObject()
{

}

// Game Loop ====================================
bool GameObject::Update()
{
	return true;
}

// Set Methods ==================================
void GameObject::SetActiveState(bool act)
{
	actived = act;
}

// Functionality ================================
void GameObject::PushComponent(const Component * cmp)
{
	components.push_back((Component*)cmp);
}

void GameObject::RemoveComponent(const Component * cmp)
{
	components.remove((Component*)cmp);
}
