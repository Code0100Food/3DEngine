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
	//Delete components
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(components[k]);
	}
	components.clear();

	//Delete childs
	size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		RELEASE(childs[k]);
	}
	childs.clear();

	//Reset parent
	parent = nullptr;
}

// Game Loop ====================================
bool GameObject::Update()
{
	bool ret = true;

	/* This update*/

	//Update childs
	ret = UpdateChilds();

	return ret;
}

bool GameObject::UpdateChilds()
{
	bool ret = true;

	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		ret = childs[k]->Update();
	}

	return ret;
}

// Set Methods ==================================
void GameObject::SetActiveState(bool act)
{
	actived = act;
}

void GameObject::SetName(const char * str)
{
	name = str;
}

void GameObject::SetParent(GameObject * target)
{
	parent = (GameObject*)target;
}

// Functionality ================================
Component * GameObject::CreateComponent(COMPONENT_TYPE c_type)
{
	Component* comp = nullptr;

	switch (c_type)
	{
	case COMP_TRANSFORMATION:
		break;
	case COMP_MESH:
		break;
	case COMP_MATERIAL:
		break;
	}

	if (comp != nullptr)comp->SetParent(this);

	return comp;
}

bool GameObject::RemoveComponent(Component * cmp)
{
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k] == cmp)
		{
			RELEASE(components[k]);

			for (uint h = k; h < size - 1; h++)
			{
				components[h] == components[h + 1];
			}

			components.pop_back();

			return true;
		}
	}

	return false;
}

bool GameObject::FindComponent(Component * cmp) const
{
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k] == cmp)return true;

	}

	return false;
}

void GameObject::AddChild(const GameObject * child)
{
	if (child != nullptr)childs.push_back((GameObject*)child);
}

bool GameObject::RemoveChild(GameObject * child, bool search_in)
{
	bool ret = false;
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k] == child)
		{
			RELEASE(childs[k]);

			for (uint h = k; h < size - 1; h++)
			{
				childs[h] == childs[h + 1];
			}

			childs.pop_back();

			ret = true;
			break;
		}
		else if (search_in)
		{
			ret = childs[k]->RemoveChild(child, search_in);
			if (ret)break;
		}
	}

	return ret;
}
