#include "ModuleScene.h"

#include "Application.h"
#include "InputManager.h"
#include "ModuleInput.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "Serializer.h"
#include "FileSystem.h"
#include "ModuleTextures.h"
#include "TimeManager.h"
#include "ModuleAudio.h"
#include "ResourcesManager.h"

// Constructors =================================
ModuleScene::ModuleScene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleScene::~ModuleScene()
{

}

// Game Loop ====================================
bool ModuleScene::Start()
{
	//Create the root game object
	root_gameobject = CreateGameObject();
	root_gameobject->SetName("Scene");

	//Set a default octree configuration
	int bound_size = 0;
	math::AABB boundaries = math::AABB(math::float3(-bound_size, -bound_size, -bound_size), math::float3(bound_size, bound_size, bound_size));
	octree.SetBoundaries(boundaries);
	octree.SetMaxObjects(1);
	
	//Set a default game timing configuration
	App->time_manager->SetGameTimeScale(1.0f);

	//Load a scene if theres a saved one
	char str[50];
	sprintf(str, "%sscene.json", LIBRARY_FOLDER);
	if (false)//!App->scene->LoadSerializedScene(str))
	{
		LOG("No default saved scene!");
		//Generate a default main camera
		GameObject* main_camera = CreateGameObject();
		main_camera->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		main_camera->CreateComponent(COMPONENT_TYPE::COMP_CAMERA);
		main_camera->SetName("Main Camera");
	}
	else LOG("Scene loaded correctly!");

	return true;
}

bool ModuleScene::SceneUpdate(float dt)
{
	bool ret = true;

	//Exit engine
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_STATE::INPUT_DOWN)
	{
		App->SetQuit();
	}
	
	//Release the game objects ready to be deleted
	uint size = objects_to_delete.size();
	for (uint k = 0; k < size; k++)
	{
		ReleaseGameObject(objects_to_delete[k], objects_to_delete[k]->GetParent());
	}
	objects_to_delete.clear();

	//Update the scene game objects
	float game_dt = App->time_manager->GetGameDT();
	if (scene_update_state == SCENE_UPDATE_STATE::PAUSE_SCENE_STATE)game_dt = 0;
	else if(scene_update_state != EDIT_SCENE_STATE)App->time_manager->AddGameTimeSinceStartup(game_dt);
	ret = root_gameobject->Update(game_dt);

	//Draw the octree
	App->renderer3D->DisableGLRenderFlags();
	octree.Draw();
	App->renderer3D->EnableGLRenderFlags();

	//Draw Gizmo
	if (App->renderer3D->GetGizmo() && selected_gameobject)
	{
		App->renderer3D->GetGizmo()->Draw();
	}

	return ret;
}

bool ModuleScene::CleanUp()
{
	RELEASE(root_gameobject);
	
	return true;
}

// Set Methods ==================================
void ModuleScene::SetSelectedGameObject(const GameObject * target)
{
	if (selected_gameobject != nullptr)selected_gameobject->SetDrawSelectedBoundingBoxState(false);
	selected_gameobject = (GameObject*)target;
	if(target != nullptr)selected_gameobject->SetDrawSelectedBoundingBoxState(true);
}

// Get Methods ==================================
GameObject * ModuleScene::GetSelectedGameObject() const
{
	return selected_gameobject;
}

SCENE_UPDATE_STATE ModuleScene::GetSceneState() const
{
	return scene_update_state;
}

// Functionality ================================
GameObject * ModuleScene::CreateGameObject()
{
	GameObject * obj = new GameObject();
	
	obj->SetParent(root_gameobject);
	
	return obj;
}

bool ModuleScene::ReleaseGameObject(GameObject * target, const GameObject * parent, bool search_in)
{
	GameObject* root = (GameObject*)parent;
	if (root == nullptr)root = root_gameobject;

	return root->RemoveChild(target, search_in);
}

void ModuleScene::SendGameObjectToRemoveVec(const GameObject * target)
{
	objects_to_delete.push_back((GameObject*)target);
}

GameObject * ModuleScene::FindGameObject(uint id) const
{
	return root_gameobject->FindChild(id);
}

GameObject * ModuleScene::CreatePrimitive(PRIMITIVE_TYPE type)
{
	GameObject* new_prim = nullptr;
	ComponentMesh* mesh = nullptr;
	
	switch (type)
	{
	case UNDEF_PRIMITIVE:
		break;
	case PRIMITIVE_CUBE:
	{
		//Generate the cube logic
		math::AABB cube_logic;
		cube_logic.minPoint = math::float3(0, 0, 0);
		cube_logic.maxPoint = math::float3(1, 1, 1);

		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Cube");

		//Create the cube mesh
		ComponentCubeMesh* cube_mesh = (ComponentCubeMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_CUBE_MESH);
		cube_mesh->SetGeometry(cube_logic);
		cube_mesh->SetResourceMesh(App->res_manager->GetPrimitiveResourceMesh(type));
		cube_mesh->SetPrimitiveType(PRIMITIVE_TYPE::PRIMITIVE_CUBE);
		mesh = cube_mesh;
	}
	break;
	case PRIMITIVE_SPHERE:
	case PRIMITIVE_SPHERE_HI:
	{
		//Generate the sphere logic
		math::Sphere sphere_logic;
		sphere_logic.r = 1;
		sphere_logic.pos = math::float3(0, 0, 0);

		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Sphere");

		//Create the sphere mesh
		ComponentSphereMesh* sphere_mesh = (ComponentSphereMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_SPHERE_MESH);
		sphere_mesh->SetGeometry(sphere_logic);
		ResourceMesh* res_mesh = nullptr;
		sphere_mesh->SetResourceMesh(App->res_manager->GetPrimitiveResourceMesh(type));
		if(type == PRIMITIVE_SPHERE)sphere_mesh->SetPrimitiveType(PRIMITIVE_TYPE::PRIMITIVE_SPHERE);
		else sphere_mesh->SetPrimitiveType(PRIMITIVE_TYPE::PRIMITIVE_SPHERE_HI);
		mesh = sphere_mesh;
	}
	break;
	case PRIMITIVE_CYLINDER:
	case PRIMITIVE_CYLINDER_HI:
	{
		//Generate the cylinder logic
		math::Cylinder cylinder_logic;
		cylinder_logic.r = 1;
		cylinder_logic.l.b = math::float3(0, 0, 0);
		cylinder_logic.l.a = math::float3(0, 1, 0);

		//Create the game object
		new_prim = CreateGameObject();
		new_prim->SetName("Cylinder");

		//Create the cylinder mesh
		ComponentCylinderMesh* cylinder_mesh = (ComponentCylinderMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_CYLINDER_MESH);
		cylinder_mesh->SetGeometry(cylinder_logic);
		cylinder_mesh->SetResourceMesh(App->res_manager->GetPrimitiveResourceMesh(type));
		if (type == PRIMITIVE_CYLINDER)cylinder_mesh->SetPrimitiveType(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER);
		else cylinder_mesh->SetPrimitiveType(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER);
		mesh = cylinder_mesh;
	}
	break;
	case PRIMITIVE_CAPSULE:
		break;
	case PRIMITIVE_FRUSTUM:
		break;
	case PRIMITIVE_GRID:
		break;
	default:
		break;
	}

	if (new_prim != nullptr && mesh != nullptr)
	{
		//Generate the game object
		new_prim->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);

		//Primitive mesh renderer
		ComponentMeshRenderer* mesh_renderer = (ComponentMeshRenderer*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
		mesh_renderer->SetTargetMesh(mesh);

		//Set cube logic in mesh
		mesh->SetupMesh();
		new_prim->AdjustBoundingBox();
	}

	return new_prim;
}

void ModuleScene::BlitConfigInfo()
{
	ImGui::Text("Octree Configuration");
	if (ImGui::Button("Recalculate", ImVec2(90, 25)))
	{
		CleanOctree();
		ReFillOctree();
	}
	ImGui::SameLine();
	if (ImGui::Button("Clean", ImVec2(80, 25)))
	{
		CleanOctree();
	}

}

void ModuleScene::BlitHierarchy()
{
	root_gameobject->BlitGameObjectHierarchy(0);
}

bool ModuleScene::GetHierarchyWinState() const
{
	return hierarchy_win_state;
}

void ModuleScene::SwapHierarchyWinState()
{
	hierarchy_win_state = !hierarchy_win_state;
}

void ModuleScene::BlitInspector()
{
	if (selected_gameobject != nullptr)
	{
		selected_gameobject->BlitGameObjectInspector();
	}
}

bool ModuleScene::GetInspectorState() const
{
	return inspector_state;
}

void ModuleScene::EnableInspector()
{
	inspector_state = true;
}

void ModuleScene::DisableInspector()
{
	inspector_state = false;
}

void ModuleScene::BlitComponentsWindow(GameObject* target)
{
	if (target != nullptr)
	{
		if (target->FindMeshComponent() == nullptr)
		{
			ImGui::Separator();

			ResourceMesh* n_mesh_resource = (ResourceMesh*)App->res_manager->BlitResourceButtonsByType(RESOURCE_TYPE::MESH_RESOURCE);
			if (n_mesh_resource != nullptr)
			{		
				//Create the cylinder mesh
				ComponentMesh* n_mesh_component = (ComponentMesh*)target->CreateComponent(COMPONENT_TYPE::COMP_MESH);
				n_mesh_component->SetResourceMesh(n_mesh_resource);
				target->AdjustBoundingBox();

				ComponentMeshRenderer* rend = (ComponentMeshRenderer*)target->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
				if (rend != nullptr)rend->SetTargetMesh(n_mesh_component);

				show_components_window = false;
			}
		}
		if (target->FindComponent(COMPONENT_TYPE::COMP_MESH_RENDERER) == nullptr)
		{
			ImGui::Separator();

			if (ImGui::Button("Mesh Renderer"))
			{
				App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);

				//Generate a mesh renderer
				ComponentMeshRenderer* mesh_rend = (ComponentMeshRenderer*)target->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
				//Find the mesh to render
				ComponentMesh* target_mesh = target->FindMeshComponent();
				//Link the elements
				if (target_mesh != nullptr)mesh_rend->SetTargetMesh(target_mesh);

				show_components_window = false;
			}
		}
		if (target->FindComponent(COMPONENT_TYPE::COMP_MATERIAL) == nullptr)
		{
			ImGui::Separator();

			ResourceMaterial* n_mat_resource = (ResourceMaterial*)App->res_manager->BlitResourceButtonsByType(RESOURCE_TYPE::MATERIAL_RESOURCE);
			if (n_mat_resource != nullptr)
			{
				//Create the cylinder mesh
				ComponentMaterial* n_material_component = (ComponentMaterial*)target->CreateComponent(COMPONENT_TYPE::COMP_MATERIAL);
				n_material_component->AddTexture(n_mat_resource);
				target->AdjustBoundingBox();

				ComponentMesh* mesh = (ComponentMesh*)target->FindMeshComponent();
				if (mesh != nullptr)mesh->SetDrawMaterial(n_material_component);

				show_components_window = false;
			}
		}

		if (target->FindComponent(COMPONENT_TYPE::COMP_CAMERA) == nullptr)
		{
			ImGui::Separator();

			if (ImGui::Button("Camera"))
			{
				App->audio->PlayFxForInput(FX_ID::CHECKBOX_FX);

				//Generate a component camera
				target->CreateComponent(COMPONENT_TYPE::COMP_CAMERA);
				show_components_window = false;
			}
		}
	}
}

bool ModuleScene::GetComponentsWinState() const
{
	return show_components_window;
}

void ModuleScene::SetComponentsWindowState(bool val)
{
	show_components_window = val;
}

void ModuleScene::BlitGameObjectHierarchyWindow(GameObject * target)
{
	bool val = true;
	ImGui::Begin(".", &val, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
	if (ImGui::Button("Remove", ImVec2(50, 20)))
	{
		objects_to_delete.push_back(target);
		App->scene->SetGameObjectHierarchyWindowState(false);
	}

	ImGui::SameLine();

	if (ImGui::Button("Clone", ImVec2(50, 20)))
	{

	}
	ImGui::End();
}

bool ModuleScene::GetGameObjectHierarchyWindowState() const
{
	return show_gameobject_hierarchy_window;
}

void ModuleScene::SetGameObjectHierarchyWindowState(bool val)
{
	show_gameobject_hierarchy_window = val;
}

bool ModuleScene::IsRoot(const GameObject * target) const
{
	return (target == root_gameobject);
}

GameObject * ModuleScene::GetRoot() const
{
	return root_gameobject;
}

void ModuleScene::PushGameObjectInOctree(GameObject * target, bool _childs)
{
	//Insert the game object if is static
	if (target->GetStatic())
	{
		octree.Insert(target, *target->GetBoundingBox());
	}

	if (_childs)
	{
		std::vector<GameObject*> childs = *target->GetChilds();
		uint size = childs.size();
		for (uint k = 0; k < size; k++)
		{
			//Insert the childs if are static
			if(childs[k]->GetStatic())PushGameObjectInOctree(childs[k], _childs);
		}
	}
}

void ModuleScene::ReFillOctree()
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		octree.AdjustBoundaries(*static_objects[k]->GetBoundingBox());
	}
	for (uint k = 0; k < size; k++)
	{
		octree.Insert(static_objects[k], *static_objects[k]->GetBoundingBox());
	}
}

void ModuleScene::CollectOctreeCandidates(math::Frustum & frustum, std::queue<GameObject*>& queue)
{
	octree.CollectCandidates(frustum, queue);
}

void ModuleScene::CleanOctree()
{
	octree.Reset();
}

void ModuleScene::InsertStaticObject(const GameObject * target)
{
	static_objects.push_back((GameObject*)target);
}

void ModuleScene::RemoveStaticObject(const GameObject * target)
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		if (static_objects[k] == target)
		{
			for (uint h = k; h < size - 1; h++)
			{
				static_objects[h] = static_objects[h + 1];
			}

			static_objects.pop_back();

			break;
		}
	}
}

void ModuleScene::HideStaticObjects()
{
	uint size = static_objects.size();
	for (uint k = 0; k < size; k++)
	{
		static_objects[k]->SetActiveState(false);
	}
}

void ModuleScene::SerializeScene() const
{
	//Create the serialized file root
	Serializer serialized_file;

	//Insert GameObjects array
	Serializer game_objects_array(serialized_file.InsertArray("GameObjects"));

	//Here we iterate all the game objects and save all the necessary data
	bool correct = true;
	std::vector<GameObject*> objs = *root_gameobject->GetChilds();
	uint size = objs.size();
	for (uint k = 0; k < size; k++)
	{
		correct = objs[k]->Save(game_objects_array);
		if (!correct)
		{
			LOG("[error] Error Serializing Scene!");
			break;
		}
	}

	if (correct)
	{
		//Save the built json file
		char* buffer = nullptr;
		uint size = serialized_file.Save(&buffer);
		App->fs->SaveFile("scene.json", buffer, size - 1, LIBRARY_FOLDER);

		RELEASE_ARRAY(buffer);
	}
}

bool ModuleScene::LoadSerializedScene(const char * path)
{
	bool ret = true;

	const JSON_Value* scene_data = json_parse_file(path);
	if (scene_data == nullptr)
	{
		LOG("[error] Invalid scene, file not found!");
		return false;
	}

	LOG("Loading Scene...");
	//Serializer whit all the new scene data
	Serializer scene(scene_data);

	//Game objects array
	Serializer objects = scene.GetArray("GameObjects");

	//Iterate all the elements
	uint size = objects.GetArraySize();
	for(uint k = 0; k < size;k++)
	{
		GameObject* new_obj = CreateGameObject();
		new_obj->Load(objects.GetArrayElement(k), objects_links, components_links);
	}

	//Link all the loaded objects
	size = objects_links.size();
	for(uint k = 0; k < size; k++)
	{
		GameObject* parent = FindGameObject(objects_links[k].second);
		if (parent != nullptr)objects_links[k].first->SetParent(parent);
	}
	
	objects_links.clear();

	if (ret)LOG("Scene Correctly Loaded!");

	return ret;
}

bool ModuleScene::InitializeScene()
{
	return root_gameobject->Start();
}

void ModuleScene::CleanScene()
{
	std::vector<GameObject*> root_objs = *root_gameobject->GetChilds();
	uint size = root_objs.size();
	for (uint k = 0; k < size; k++)
	{
		ReleaseGameObject(root_objs[k], root_gameobject);
	}
	selected_gameobject = nullptr;
}

void ModuleScene::PlayGame()
{
	if (scene_update_state == PLAY_SCENE_STATE)
	{
		scene_update_state = EDIT_SCENE_STATE;
		App->scene->CleanScene();
		char str[50];
		sprintf(str, "%sscene.json", LIBRARY_FOLDER);
		App->scene->LoadSerializedScene(str);
		LOG("Game Ended!");
		App->time_manager->SetGameTimeSinceStartup(0.0f);
		App->textures->play_icon_id = App->textures->play_icon;
	}
	else
	{
		scene_update_state = PLAY_SCENE_STATE;
		App->scene->SerializeScene();
		App->scene->InitializeScene();
		App->textures->play_icon_id = App->textures->play_click_icon;
		LOG("Game Started!");
	}
}

void ModuleScene::PauseGame()
{
	if (scene_update_state != PLAY_SCENE_STATE && scene_update_state != PAUSE_SCENE_STATE)return;

	if (scene_update_state == PAUSE_SCENE_STATE)
	{
		scene_update_state = PLAY_SCENE_STATE;
		App->textures->pause_icon_id = App->textures->pause_icon;
		LOG("Game Unpaused!");
	}
	else
	{
		scene_update_state = PAUSE_SCENE_STATE;
		App->textures->pause_icon_id = App->textures->pause_click_icon;
		LOG("Game Paused!");
	}
}

void ModuleScene::NextGameFrame()
{
	scene_update_state = NEXT_SCENE_STATE;
}
