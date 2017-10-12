#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"

class Scene : public Module
{
public:

	Scene(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~Scene();

public:

	update_status	Update(float dt)final;

};
#endif
