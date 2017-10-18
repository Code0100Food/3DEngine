#include "ModuleScene.h"

#include "Application.h"
#include "InputManager.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"

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
