#include "ModuleScene.h"
#include "Application.h"
#include "InputManager.h"

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
	root_gameobject->SetName("Root Object");

	return true;
}

update_status ModuleScene::Update(float dt)
{
	bool ret = true;

	//Exit engine
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_STATE::INPUT_DOWN)
	{
		App->SetQuit();
	}
	
	//Update the scene game objects
	ret = root_gameobject->Update();

	if (!ret)return update_status::UPDATE_ERROR;

	return update_status::UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	RELEASE(root_gameobject);
	
	return true;
}

// Functionality ================================
GameObject * ModuleScene::CreateGameObject()
{
	return new GameObject();
}

bool ModuleScene::RemoveGameObject(GameObject * target, const GameObject * parent, bool search_in)
{
	GameObject* root = (GameObject*)parent;
	if (root == nullptr)root = root_gameobject;

	return root->RemoveChild(target, search_in);
}