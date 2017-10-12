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
update_status ModuleScene::Update(float dt)
{
	bool ret = true;

	//Exit engine
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_STATE::INPUT_DOWN)
	{
		App->SetQuit();
	}
	
	//Update the scene game objects
	uint size = objects.size();
	for (uint k = 0; k < size; k++)
	{
		ret = objects[k]->Update();
	}

	if (!ret)return update_status::UPDATE_ERROR;

	return update_status::UPDATE_CONTINUE;
}

GameObject * ModuleScene::CreateGameObject(bool push_at_vec)
{
	GameObject* n_obj = new GameObject();
	if (push_at_vec)objects.push_back(n_obj);
	
	return n_obj;
}
