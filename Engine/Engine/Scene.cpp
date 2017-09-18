#include "Scene.h"

// Constructors =================================
Scene::Scene(Application * parent, bool start_enabled) : Module(parent, start_enabled)
{

}

// Destructors ==================================
Scene::~Scene()
{

}

// Game Loop ====================================
bool Scene::Start()
{
	return true;
}

update_status Scene::Update(float dt)
{
	LOG("Scene Update!");
	return update_status::UPDATE_CONTINUE;
}