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

bool Component::Start()
{
	return true;
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

bool Component::GetActive() const
{
	return actived;
}

// Functionality ================================
void Component::BlitComponentInspector()
{
	ImGui::Text("Undef Component");
	ImGui::Checkbox("Active", &actived);
}

bool Component::Save(JSON_Object * root) const
{
	return true;
}

bool Component::Load(const JSON_Object * root)
{
	return true;
}
