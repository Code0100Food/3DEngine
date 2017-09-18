#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"

class Scene : public Module
{
public:

	Scene(Application* parent, bool start_enabled = true);
	~Scene();

public:

	bool			Start() final;
	update_status	Update(float dt)final;

public: 

	/*Scene data*/

};
#endif
