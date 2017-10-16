#include "GameObject.h"
#include "Component.h"
#include "Application.h"
#include "ModuleScene.h"

// Constructors =================================
GameObject::GameObject()
{

}

GameObject::GameObject(const GameObject & cpy) :actived(cpy.actived), name(cpy.name), parent(cpy.parent)
{
	//Clone all the components
	uint size = cpy.components.size();
	for (uint k = 0; k < size; k++)
	{
		components.push_back(CloneComponent(cpy.components[k]));
	}

	//Clone all the childs
	size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		childs.push_back(new GameObject(*cpy.childs[k]));
	}

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
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k]->GetActive())
		{
			ret = components[k]->Update();
		}
	}

	size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetActive())
		{
			ret = childs[k]->Update();
		}
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
	if (parent != nullptr)
	{
		parent->PopChild(this);
	}

	if (target != nullptr)
	{
		target->AddChild(this);
	}

	parent = (GameObject*)target;
}

// Get Methods ==================================
bool GameObject::GetActive()
{
	return actived;
}

// Functionality ================================
Component * GameObject::CreateComponent(COMPONENT_TYPE c_type)
{
	Component* comp = nullptr;

	switch (c_type)
	{
	case COMP_TRANSFORMATION:	comp = new ComponentTransform();	break;
	case COMP_MESH:				comp = new ComponentMesh();			break;
	case COMP_MATERIAL:			comp = new ComponentMaterial();		break;
	case COMP_MESH_RENDERER:	comp = new ComponentMeshRenderer();	break;
	case COMP_CAMERA:			comp = new ComponentCamera();		break;
	}

	if (comp != nullptr)
	{
		comp->SetParent(this);
		components.push_back(comp);
	}

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

Component * GameObject::FindComponent(COMPONENT_TYPE type) const
{
	Component* cmp = nullptr;
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k]->GetType() == type)
		{
			cmp = components[k];
			break;
		}
	}

	return cmp;
}

Component * GameObject::CloneComponent(const Component * target) const
{
	Component* new_c = nullptr;

	switch (target->GetType())
	{
	case COMP_TRANSFORMATION:		new_c = new ComponentTransform(*(const ComponentTransform*)target);			break;
	case COMP_MESH:					new_c = new ComponentMesh(*(const ComponentMesh*)target);					break;
	case COMP_MATERIAL:				new_c = new ComponentMaterial(*(const ComponentMaterial*)target);			break;
	case COMP_MESH_RENDERER:		new_c = new ComponentMeshRenderer(*(const ComponentMeshRenderer*)target);	break;
	}

	return new_c;
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

bool GameObject::PopChild(GameObject * child, bool search_in)
{
	bool ret = false;
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k] == child)
		{
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
			ret = childs[k]->PopChild(child, search_in);
			if (ret)break;
		}
	}

	return ret;
}

void GameObject::BlitGameObjectHierarchy()
{
	bool op = ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick);

	if (ImGui::IsItemClicked() && ImGui::IsItemHovered())
	{
		App->scene->EnableInspector();
		App->scene->SetSelectedGameObject(this);
	}

	if(op)
	{
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			childs[k]->BlitGameObjectHierarchy();
		}
		ImGui::TreePop();
	}

}

void GameObject::BlitGameObjectInspector()
{
	//Blit game object base data
	ImGui::Checkbox("##object_active", &actived);
	ImGui::SameLine();
	ImGui::Text(name.c_str());

	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		components[k]->BlitComponentInspector();
	}
}
