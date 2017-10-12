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

	bool			Start()final;
	update_status	Update(float dt)final;
	bool			CleanUp()final;

private:

	GameObject*		root_gameobject = nullptr;

public:

	GameObject* CreateGameObject();
	bool		RemoveGameObject(GameObject* target, const GameObject* parent, bool search_in = true);
};
#endif
