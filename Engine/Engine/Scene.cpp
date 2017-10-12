#include "Scene.h"
#include "Application.h"
#include "InputManager.h"

// Constructors =================================
Scene::Scene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
Scene::~Scene()
{

}

// Game Loop ====================================
update_status Scene::Update(float dt)
{
	
	//Exit engine
	if (App->input_manager->GetEvent(ESCAPE) == INPUT_STATE::INPUT_DOWN)
	{
		App->SetQuit();
	}

	return update_status::UPDATE_CONTINUE;
}