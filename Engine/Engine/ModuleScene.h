#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "GameObject.h"

class ModuleScene : public Module
{
public:

	ModuleScene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleScene();

public:

	update_status	Update(float dt)final;

private:

	std::vector<GameObject*> objects;

public:

	GameObject* CreateGameObject(bool push_at_vec = true);

};
#endif
