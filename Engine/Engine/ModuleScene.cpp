#include "ModuleScene.h"

#include "Application.h"
#include "InputManager.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

#include "Cube_.h"
#include "SphereGenerator.h"

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

	//Generate a default main camera
	GameObject* test = CreateGameObject();
	test->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
	test->CreateComponent(COMPONENT_TYPE::COMP_CAMERA);
	test->SetName("Main Camera");

	//Set a default octree configuration
	int bound_size = 10;
	math::AABB boundaries = math::AABB(math::float3(-bound_size, -bound_size, -bound_size), math::float3(bound_size, bound_size, bound_size));
	octree.SetBoundaries(boundaries);


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
	
	//Update the scene game objects
	ret = root_gameobject->Update();

	//Draw the octree
	App->renderer3D->DisableGLRenderFlags();
	octree.Draw();
	App->renderer3D->EnableGLRenderFlags();

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
	selected_gameobject = (GameObject*)target;
}

// Get Methods ==================================
GameObject * ModuleScene::GetSelectedGameObject() const
{
	return selected_gameobject;
}

// Functionality ================================
GameObject * ModuleScene::CreateGameObject()
{
	GameObject * obj = new GameObject();
	
	obj->SetParent(root_gameobject);
	
	return obj;
}

bool ModuleScene::RemoveGameObject(GameObject * target, const GameObject * parent, bool search_in)
{
	GameObject* root = (GameObject*)parent;
	if (root == nullptr)root = root_gameobject;

	return root->RemoveChild(target, search_in);
}

GameObject * ModuleScene::CreatePrimitive(PRIMITIVE_TYPE type)
{
	GameObject* new_prim = nullptr;

	switch (type)
	{
	case UNDEF_PRIMITIVE:
		break;
	case PRIMITIVE_POINT:
		break;
	case PRIMITIVE_LINE:
		break;
	case PRIMITIVE_PLANE:
		break;
	case PRIMITIVE_CUBE:
	{
		//Generate the cube logic
		SphereGenerator sphere;
		sphere.SetRad(8);
		sphere.SetPosition(math::float3(0, 0, 0));
		sphere.SetDivisions(4);
		std::pair<std::vector<uint>, std::vector<Vertex>> data = sphere.Generate();

		//Generate the game object
		new_prim = CreateGameObject();
		new_prim->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
		
		ComponentMesh* mesh = (ComponentMesh*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_MESH);
		
		ComponentMeshRenderer* mesh_renderer = 	(ComponentMeshRenderer*)new_prim->CreateComponent(COMPONENT_TYPE::COMP_MESH_RENDERER);
		mesh_renderer->SetTargetMesh(mesh);

		//Set cube logic in mesh
		mesh->SetIndices(data.first);
		mesh->SetVertices(data.second);
		mesh->SetupMesh();
	}
		break;
	case PRIMITIVE_SPHERE:
		break;
	case PRIMITIVE_CYLINDER:
		break;
	case PRIMITIVE_RAY:
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

	return new_prim;
}

void ModuleScene::BlitHierarchy()
{
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.15f, (App->window->GetHeight() - 23) * 0.5f), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(0, (App->window->GetHeight() * 0.5f)), ImGuiCond_Always);

	ImGui::Begin("Hierarchy", &hierarchy_win_state, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Hierarchy");

	root_gameobject->BlitGameObjectHierarchy(0);
	
	ImGui::End();
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
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.25f, (App->window->GetHeight() - 23) * 0.5f), ImGuiCond_Always);
	ImGui::SetNextWindowPos(ImVec2(App->window->GetWidth() * 0.15f, (App->window->GetHeight() * 0.5f)), ImGuiCond_Always);

	ImGui::Begin("Inspector", &inspector_state, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus);
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Inspector");

	if (selected_gameobject != nullptr)
	{
		selected_gameobject->BlitGameObjectInspector();
	}

	ImGui::End();
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

bool ModuleScene::IsRoot(const GameObject * target) const
{
	return (target == root_gameobject);
}

GameObject * ModuleScene::GetRoot() const
{
	return root_gameobject;
}
