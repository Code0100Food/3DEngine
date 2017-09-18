#ifndef _SCENE_H_
#define _SCENE_H_

#include "Module.h"
#include "MathGeoLib/Algorithm/Random/LCG.h"
#include "MathGeoLib/Time/Clock.h"

class Scene : public Module
{
public:

	Scene(Application* parent, bool start_enabled = true);
	~Scene();

public:

	bool			Start() final;
	update_status	Update(float dt)final;

public: 

	math::LCG*	rand_num_generator = nullptr;
	std::string	rand_num_string = "NaN";

};
#endif
