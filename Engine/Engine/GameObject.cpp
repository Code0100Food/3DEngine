#include "GameObject.h"

#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"

#include "Application.h"
#include "ModuleScene.h"
#include "ModuleWindow.h"
#include "GeometryManager.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "TimeManager.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleScene.h"
#include "Serializer.h"
#include "ResourcesManager.h"

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
		components[k]->SetParent(this);
	}

	//Clone all the childs
	size = cpy.childs.size();
	for (uint k = 0; k < size; k++)
	{
		GameObject* clone = new GameObject(*cpy.childs[k]);
		clone->SetParent(this);
	}

	AdjustBoundingBox();
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

	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		ret = components[k]->Start();
		if (!ret)break;
	}

	if (ret)
	{
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			ret = childs[k]->Start();
			if (!ret)break;
		}
	}

	return ret;
}

bool GameObject::Update(float dt)
{
	bool ret = true;

	/* This update*/
	if (draw_bounding_box || draw_selected_bounding_box)DrawBoundingBox();
	
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

void GameObject::SetHideState(bool new_hide_state)
{
	hide = new_hide_state;
}

void GameObject::SetStatic(bool st)
{
	static_ = st;

	if (static_)
	{
		App->scene->InsertStaticObject(this);
	}
	else
	{
		App->scene->RemoveStaticObject(this);
	}

	uint size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		childs[k]->SetStatic(st);
	}
}

void GameObject::SetDrawSelectedBoundingBoxState(bool val)
{
	draw_selected_bounding_box = val;
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

void GameObject::SetMaterial(ResourceMaterial * mat)
{
	//Get the component material
	ComponentMaterial* mat_comp = (ComponentMaterial*)FindComponent(COMPONENT_TYPE::COMP_MATERIAL);
	if (mat_comp == nullptr)
	{
		mat_comp = (ComponentMaterial*)CreateComponent(COMPONENT_TYPE::COMP_MATERIAL);
		ComponentMesh* target_mesh = FindMeshComponent();
		target_mesh->SetDrawMaterial((ComponentMaterial*)FindComponent(COMPONENT_TYPE::COMP_MATERIAL));
	}
	mat_comp->ClearTextures();
	mat_comp->AddTexture(mat);
}

void GameObject::SetMesh(ResourceMesh * mesh)
{

}

// Get Methods ==================================
bool GameObject::GetActive() const
{
	return actived;
}

bool GameObject::GetHide() const
{
	return hide;
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

GameObject * GameObject::GetParent() const
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

const char * GameObject::GetName() const
{
	return name.c_str();
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
	case COMP_SCRIPT:			comp = new ComponentScript();			break;
	}

	if (comp != nullptr)
	{
		if (c_type == COMP_TRANSFORMATION)
		{
			components.push_back(comp);
			uint size = components.size() - 1;
			for (uint k = 0; k < size; k++)
			{
				components[k + 1] = components[k];
			}
			components[0] = comp;
		}
		else
		{
			components.push_back(comp);
		}
		comp->SetParent(this);
		comp->Start();
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
			//First reset the related data
			bool mesh_type = components[k]->IsMeshType();
			components[k]->UnLinkComponent();
			
			//Release the component
			RELEASE(components[k]);

			//Update the components vector
			for (uint h = k; h < size - 1; h++)
			{
				components[h] = components[h + 1];
			}
			components.pop_back();

			//Recalculate the bounding box of the correct parent
			if (mesh_type)
			{
				GameObject* root_target_parent = parent;
				GameObject*	target_parent = this;

				while (!root_target_parent->IsRoot())
				{
					target_parent = root_target_parent;
					root_target_parent = target_parent->parent;
				}
				target_parent->AdjustBoundingBox();
			}

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

void GameObject::FindComponents(COMPONENT_TYPE type, std::vector<Component*>* vec)
{
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		if (components[k]->GetType() == type)
		{
			vec->push_back(components[k]);
		}
	}

	size = childs.size();
	for (uint k = 0; k < size; k++)
	{
		childs[k]->FindComponents(type, vec);
	}
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
	case COMP_SCRIPT:				new_c = new ComponentScript(*(const ComponentScript*)target);				break;
	case COMP_MESH:					new_c = new ComponentMesh(*(const ComponentMesh*)target);					break;
	case COMP_MATERIAL:				new_c = new ComponentMaterial(*(const ComponentMaterial*)target);			break;
	case COMP_MESH_RENDERER:		new_c = new ComponentMeshRenderer(*(const ComponentMeshRenderer*)target);	break;
	case COMP_CAMERA:				new_c = new ComponentCamera(*(const ComponentCamera*)target);				break;
	case COMP_CUBE_MESH:			new_c = new ComponentCubeMesh(*(const ComponentCubeMesh*)target);			break;
	case COMP_SPHERE_MESH:			new_c = new ComponentSphereMesh(*(const ComponentSphereMesh*)target);		break;
	case COMP_CYLINDER_MESH:		new_c = new ComponentCylinderMesh(*(const ComponentCylinderMesh*)target);	break;
	}

	return new_c;
}

void GameObject::CloneGameObject()
{
	GameObject* new_gameobject = new GameObject(*this);
	new_gameobject->parent->AddChild(new_gameobject);
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
			//If the object is static remove it from the static vec
			if (childs[k]->GetStatic())App->scene->RemoveStaticObject(childs[k]);

			RELEASE(childs[k]);

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
	GameObject* found = nullptr;

	uint size = childs.size();
	for (uint k = 0; k < size && found == nullptr; k++)
	{
		if (childs[k]->id == id)
		{
			found = childs[k];
			break;
		}
		
		found = childs[k]->FindChild(id);
	}

	return found;
}

std::vector<GameObject*>* GameObject::GetChilds()
{
	return &childs;
}

const std::vector<GameObject*>* GameObject::GetChildsConst()const
{
	return &childs;
}

void GameObject::RecalculateID()
{
	id = App->randomizer->Int();
}

void GameObject::BlitGameObjectHierarchy(uint index)
{
	//Check the tree node flags
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_OpenOnDoubleClick;
	if (childs.empty())flags += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Leaf;
	if (this == App->scene->GetSelectedGameObject())flags += ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_Selected;
	
	//Generate the node
	char name_str[250];
	sprintf_s(name_str, 250, "%s##%i", name.c_str(), index);
	bool op = ImGui::TreeNodeEx(name_str, flags);

	//Tree node input
	if (!IsRoot())
	{
		bool is_sel = (App->scene->GetSelectedGameObject() == this);


		if (ImGui::IsItemHovered())
		{
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN)
			{
				App->scene->EnableInspector();
				App->scene->SetSelectedGameObject(this);
				App->scene->SetGameObjectHierarchyWindowState(false);
			}
		}
	}

	//Right click
	if (ImGui::IsMouseClicked(1) && ImGui::IsItemRectHovered())
	{
		char name[150];
		sprintf(name, "GameObject Options##%i", id);
		ImGui::OpenPopup(name);
	}

	//Right click on the Gameobject hierarchy
	HandleRightClickInput();

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

	ImGui::SameLine();
	
	//Release button
	if (ImGui::ImageButton((ImTextureID)App->textures->garbage_icon,ImVec2(20,20)))
	{
		App->scene->SendGameObjectToRemoveVec(this);
		App->scene->SetSelectedGameObject(nullptr);
		App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);
		return;
	}

	//Static Object
	if (ImGui::Checkbox("Static##object_static", &static_))
	{
		SetStatic(static_);
	}

	//Bounding box
	ImGui::Checkbox("Bounding Box", &draw_bounding_box);
	
	//Components inspectors
	uint size = components.size();
	for (uint k = 0; k < size; k++)
	{
		//Show component inspector data
		components[k]->BlitComponentInspector();

		if (components[k]->GetType() != COMPONENT_TYPE::COMP_TRANSFORMATION)
		{
			char str[40];
			sprintf(str, "Delete Component##%i", k);
			if (ImGui::Button(str))
			{
				App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);
				RemoveComponent(components[k]);
				break;
			}
		}
	}

	//Components factory
	ImGui::Separator();
	
	if (ImGui::Button("Add Component", ImVec2(App->window->GetWidth() * 0.24f, 25)))
	{
		App->scene->SetComponentsWindowState(!App->scene->GetComponentsWinState());
		App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);
	}

	if (App->scene->GetComponentsWinState())
	{
		App->scene->BlitComponentsWindow(this);
	}

	//Add a margin to scroll
	ImGui::NewLine();ImGui::NewLine();ImGui::NewLine();ImGui::NewLine();
}

void GameObject::HandleRightClickInput()
{
	char name[150];
	sprintf(name, "GameObject Options##%i", id);

	if (ImGui::BeginPopup(name))
	{
		ImGui::Text("%s", this->name.c_str());
		ImGui::Separator();

		//Remove Selectable
		BlitRemove();
		
		//Add empty selectable
		BlitAddEmpty();

		//Add 3D object Selectable
		Blit3DObject();
		
		//Add Prefabs Selectable
		BlitPrefabs();

		//Clone this Gameobject
		BlitClone();

		ImGui::EndPopup();
	}
}

void GameObject::BlitRemove()
{
	//If is root can't remove
	if (!App->scene->IsRoot(this))
	{
		if (ImGui::Selectable("Remove"))
		{
			App->scene->SendGameObjectToRemoveVec(this);
			App->scene->SetSelectedGameObject(nullptr);
			App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "Remove");
	}
}

void GameObject::BlitClone()
{
	//If is root can't clone
	if (!App->scene->IsRoot(this))
	{
		if (ImGui::Selectable("Clone"))
		{
			CloneGameObject();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.3f, 0.3f, 0.3f, 1.0f), "Clone");
	}
}

void GameObject::BlitAddEmpty()
{
	if (ImGui::Selectable("Add Empty"))
	{
		GameObject* new_child = new GameObject();
		new_child->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		new_child->SetParent(this);
	}
}

void GameObject::Blit3DObject()
{
	if (ImGui::BeginMenu("3D Objects"))
	{
		if (ImGui::Selectable("Cube"))
		{
			GameObject* new_primitive = App->scene->CreatePrimitive(PRIMITIVE_CUBE);
			new_primitive->SetParent(this);
		}

		if (ImGui::BeginMenu("Sphere"))
		{
			if (ImGui::Selectable("Low Poly"))
			{
				GameObject* new_primitive = App->scene->CreatePrimitive(PRIMITIVE_SPHERE);
				new_primitive->SetParent(this);
			}

			if (ImGui::Selectable("High Poly"))
			{
				GameObject* new_primitive = App->scene->CreatePrimitive(PRIMITIVE_SPHERE_HI);
				new_primitive->SetParent(this);
			}


			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Cylinder"))
		{
			if (ImGui::Selectable("Low Poly"))
			{
				GameObject* new_primitive = App->scene->CreatePrimitive(PRIMITIVE_CYLINDER);
				new_primitive->SetParent(this);
			}

			if (ImGui::Selectable("High Poly"))
			{
				GameObject* new_primitive = App->scene->CreatePrimitive(PRIMITIVE_CYLINDER_HI);
				new_primitive->SetParent(this);
			}

			ImGui::EndMenu();
		}
		ImGui::EndMenu();
	}
}

void GameObject::BlitPrefabs()
{
	App->res_manager->BlitPrefabsMenu(this);
}

GameObject * GameObject::BlitAsSelectable(bool included)
{
	if (included)
	{
		if (ImGui::Selectable(name.c_str()))
		{
			return this;
		}
	}

	uint size = childs.size();
	for(uint k = 0; k < size; k++)
	{
		GameObject* ret = childs[k]->BlitAsSelectable();
		if (ret != nullptr)return ret;
	}

	return nullptr;
}

void GameObject::DrawBoundingBox()
{
	App->renderer3D->DisableGLRenderFlags();

	if (draw_selected_bounding_box)bounding_box.Draw(3.5f, App->geometry->selected_bounding_box_color);
	else bounding_box.Draw(2.5f, App->geometry->bounding_box_color);
	
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
	if (trans != nullptr)tmp.TransformAsAABB(trans->GetInheritedTransform().Transposed());
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
			if ((*child)->bounding_box.IsFinite())
			{
				math::float3 aabb_points[8];
				(*child)->bounding_box.GetCornerPoints(aabb_points);

				for (int i = 0; i < 8; i++)
					childs_aabb_points.push_back(aabb_points[i]);
			}
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
		
		if (mesh != nullptr && !mesh->MeshResourceIsNull())
		{
			//Collect all the vertex and bb corners
			mesh->GetVertexPositions(v_pos);
		}
		else if(v_pos.empty())bounding_box.SetNegativeInfinity();

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

bool GameObject::Load(Serializer& data, std::vector<std::pair<GameObject*, uint>>& links, std::vector<std::pair<Component*, uint>>& components_links, std::vector<Component*>& loaded_cmps)
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
		loaded_cmps.push_back(component);
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

	//Insert the object in the octree if is a static one
	if (static_)
	{
		App->scene->InsertStaticObject(this);
	}

	return ret;
}
