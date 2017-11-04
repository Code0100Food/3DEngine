#include "GameObject.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleScene.h"
#include "GeometryManager.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "TimeManager.h"

#include "Serializer.h"

// Constructors =================================
GameObject::GameObject()
{
	//Generate id
	id = App->randomizer->Int();
}

GameObject::GameObject(const GameObject & cpy) :actived(cpy.actived), name(cpy.name), parent(cpy.parent), bounding_box(cpy.bounding_box), draw_bounding_box(cpy.draw_bounding_box)
{
	//Generate id
	id = App->randomizer->Int();

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
bool GameObject::Start()
{
	bool ret = true;

	bounding_box.SetNegativeInfinity();

	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		ret = childs[k]->Start();
		if (!ret)break;
	}
		
	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	/* This update*/
	if (draw_bounding_box)DrawBoundingBox();

	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k]->GetActive())
		{
			ret = components[k]->Update(dt);
		}
	}

	size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->GetActive())
		{
			ret = childs[k]->Update(dt);
		}
	}

	UpdateBoundingBox();

	return ret;
}

// Set Methods ==================================
void GameObject::SetActiveState(bool act)
{
	actived = act;
}

void GameObject::SetStatic(bool st)
{
	static_ = st;
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
bool GameObject::GetActive() const
{
	return actived;
}

bool GameObject::GetStatic() const
{
	return static_;
}

float GameObject::GetBoundingBoxDiagonalSize() const
{
	math::float3 vec = bounding_box.maxPoint - bounding_box.minPoint;
	bool ret = (!isnan(vec.x) && !isnan(vec.y) && !isnan(vec.z));
	if (!ret)return -1;
	else return abs(vec.Length());
}

uint GameObject::GetID() const
{
	return id;
}

const GameObject * GameObject::GetParent() const
{
	return parent;
}

bool GameObject::IsRoot() const
{
	return App->scene->IsRoot(this);
}

bool GameObject::IsSelectedObject() const
{
	return (this == App->scene->GetSelectedGameObject());
}

// Functionality ================================
Component * GameObject::CreateComponent(COMPONENT_TYPE c_type)
{
	Component* comp = nullptr;

	switch (c_type)
	{
	case COMP_TRANSFORMATION:	comp = new ComponentTransform();		break;
	case COMP_MESH:				comp = new ComponentMesh();				break;
	case COMP_PRIMITIVE_MESH:	comp = new ComponentPrimitiveMesh();	break;
	case COMP_CUBE_MESH:		comp = new ComponentCubeMesh();			break;
	case COMP_SPHERE_MESH:		comp = new ComponentSphereMesh();		break;
	case COMP_CYLINDER_MESH:	comp = new ComponentCylinderMesh();		break;
	case COMP_MATERIAL:			comp = new ComponentMaterial();			break;
	case COMP_MESH_RENDERER:	comp = new ComponentMeshRenderer();		break;
	case COMP_CAMERA:			comp = new ComponentCamera();			break;
	}

	if (comp != nullptr)
	{
		comp->SetParent(this);
		comp->Start();
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

Component * GameObject::FindComponent(uint id) const
{
	Component* cmp = nullptr;
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k]->GetID() == id)
		{
			cmp = components[k];
			break;
		}
	}

	return cmp;
}

ComponentMesh * GameObject::FindMeshComponent() const
{
	ComponentMesh* cmp = nullptr;
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (	components[k]->GetType() == COMP_MESH ||
				components[k]->GetType() == COMP_CUBE_MESH ||
				components[k]->GetType() == COMP_SPHERE_MESH ||
				components[k]->GetType() == COMP_CYLINDER_MESH ||
				components[k]->GetType() == COMP_FRUSTUM_MESH)
		{
			cmp = (ComponentMesh*)components[k];
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
				childs[h] = childs[h + 1];
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

GameObject * GameObject::FindChild(uint id) const
{
	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		if (childs[k]->id == id)return childs[k];
		else childs[k]->FindChild(id);
	}

	return nullptr;
}

std::vector<GameObject*>* GameObject::GetChilds()
{
	return &childs;
}

void GameObject::BlitGameObjectHierarchy(uint index)
{
	char name_str[40];
	sprintf_s(name_str, 40, "%s##%i", name.c_str(), index);
	bool op = ImGui::TreeNodeEx(name_str, ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick);

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
			childs[k]->BlitGameObjectHierarchy(k);
		}
		ImGui::TreePop();
	}

}

void GameObject::BlitGameObjectInspector()
{
	//Blit game object base data
	//Enable / disable
	ImGui::Checkbox("##object_active", &actived);
	ImGui::SameLine();
	
	//Name
	if (ImGui::InputText("Name", (char*)name.c_str(), 20, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
	{
		App->audio->PlayFxForInput(FX_ID::APPLY_FX);
	}
	
	//Static Object
	ImGui::Checkbox("Static##object_static", &static_);

	//Bounding box
	ImGui::Checkbox("Bounding Box", &draw_bounding_box);
	
	//Components inspectors
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		components[k]->BlitComponentInspector();
	}

	//Add a margin to scroll
	ImGui::NewLine();ImGui::NewLine();ImGui::NewLine();ImGui::NewLine();
}

void GameObject::DrawBoundingBox()
{
	App->renderer3D->DisableGLRenderFlags();

	bounding_box.Draw(4.0f, App->geometry->bounding_box_color);

	App->renderer3D->EnableGLRenderFlags();
}

math::AABB * GameObject::GetBoundingBox()
{
	return &bounding_box;
}

math::AABB GameObject::GetInheritTransformedBoundingBox()
{
	math::AABB tmp = original_bounding_box;
	ComponentTransform* trans = (ComponentTransform*)FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	if (trans != nullptr)tmp.TransformAsAABB(trans->GetInheritedTransform());
	return tmp;
}

math::AABB GameObject::GetTransformedBoundingBox()
{
	math::AABB tmp = original_bounding_box;
	ComponentTransform* trans = (ComponentTransform*)FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	if (trans != nullptr)tmp.TransformAsAABB(trans->GetTransform());
	return tmp;
}

void GameObject::UpdateBoundingBox()
{
	if (App->scene->IsRoot(this))
	{
		return;
	}

	if (!childs.empty())
	{
		for (std::vector<GameObject*>::iterator child = childs.begin(); child != childs.end(); child++)
		{
			math::float3 aabb_points[8];
			(*child)->bounding_box.GetCornerPoints(aabb_points);

			for (int i = 0; i < 8; i++)
				childs_aabb_points.push_back(aabb_points[i]);
		}

		if (FindComponent(COMPONENT_TYPE::COMP_MESH))
		{
			math::float3 my_points[8];
			GetInheritTransformedBoundingBox().GetCornerPoints(my_points);

			for (int i = 0; i < 8; i++)
				childs_aabb_points.push_back(my_points[i]);
		}

		bounding_box = math::AABB::MinimalEnclosingAABB(childs_aabb_points.data(), childs_aabb_points.size());

		childs_aabb_points.clear();
	}
	else
	{
		bounding_box = GetInheritTransformedBoundingBox();
	}
}

std::pair<math::float3, math::float3> GameObject::AdjustBoundingBox(bool all_childs)
{
	uint ch_size = childs.size();
	
	std::vector<std::pair<math::float3, math::float3>> childs_bb_vertex;

	for (uint k = 0; k < ch_size; k++)
	{
		childs_bb_vertex.push_back(childs[k]->AdjustBoundingBox(all_childs));
	}

	if (App->scene->IsRoot(parent) || all_childs)
	{
		//Reset bounding box
		bounding_box.SetNegativeInfinity();

		//Collect childs bb corners
		std::vector<math::float3> v_pos;
		uint pairs_size = childs_bb_vertex.size();
		for (uint k = 0; k < pairs_size; k++)
		{
			v_pos.push_back(childs_bb_vertex[k].first);
			v_pos.push_back(childs_bb_vertex[k].second);
		}

		ComponentMesh* mesh = FindMeshComponent();
		
		if (mesh != nullptr)
		{
			//Collect all the vertex and bb corners
			v_pos = mesh->GetVertexPositions();
		}

		//Build bounding box
		bounding_box.Enclose(v_pos.data(), v_pos.size());
	}

	//Build the pair of this game object bounding box
	std::pair<math::float3, math::float3> pair;
	pair.first = bounding_box.minPoint;
	pair.second = bounding_box.maxPoint;

	original_bounding_box = bounding_box;

	return pair;
}

bool GameObject::Save(Serializer& array_root) const
{
	bool ret = false;

	//Serializer where all the data of the game object is built
	Serializer obj_data;

	//Insert active
	obj_data.InsertBool("actived", actived);
	//Insert object name
	ret = obj_data.InsertString("name", name.c_str());
	//Insert object id
	ret = obj_data.InsertInt("id", id);
	//Insert parent id
	if(parent!= nullptr && !parent->IsRoot())ret = obj_data.InsertInt("parent_id", parent->GetID());
	//Insert static
	ret = obj_data.InsertBool("static", static_);

	//Save all the game object components

	//Insert GameObjects array
	Serializer components_array(obj_data.InsertArray("Components"));

	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		ret = components[k]->Save(components_array);
		if (!ret)break;
	}

	//Save the built data in the game objects array
	ret = array_root.InsertArrayElement(obj_data);

	//Save the childs
	size = childs.size();
	for(uint k = 0; k < size; k++)
	{
		ret = childs[k]->Save(array_root);
		if (!ret)break;
	}

	return ret;
}

bool GameObject::Load(Serializer& data, std::vector<std::pair<GameObject*, uint>>& links, std::vector<std::pair<Component*, uint>>& components_links)
{
	bool ret = true;

	//Get active
	actived = data.GetBool("actived");
	//Get object name
	name = data.GetString("name");
	//Get object id
	id = data.GetInt("id");
	//Get parent id
	uint parent_id = data.GetInt("parent_id");
	if (parent_id != 0)links.push_back(std::pair<GameObject*, uint>(this, parent_id));
	//Insert static
	static_ = data.GetBool("static");

	//Load all the components
	Serializer components_data = data.GetArray("Components");
	uint size = components_data.GetArraySize();

	for (uint k = 0; k < size; k++)
	{
		Serializer comp_data = components_data.GetArrayElement(k);
		Component* component = CreateComponent(StrToComponentType(comp_data.GetString("type")));
		ret = component->Load(comp_data, components_links);
		if (!ret)break;
	}

	//Link all the loaded components
	size = components_links.size();
	for (uint k = 0; k < size; k++)
	{
		components_links[k].first->LinkComponent(FindComponent(components_links[k].second));
	}

	//Clear the components links vector
	components_links.clear();

	//Adjust the bounding box
	AdjustBoundingBox();

	return ret;
}
