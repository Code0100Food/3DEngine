#include "Scene.h"
#include "ModulePhysics3D.h"

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
	//test = new math::LCG::LCG();
	//math::Clock clock;
	LOG("%i",clock.Sec());
	//test = new LCG();
	
	return true;
}

update_status Scene::Update(float dt)
{
	LOG("%i", clock.Sec());
	LOG("Scene Update!");
	return update_status::UPDATE_CONTINUE;
}