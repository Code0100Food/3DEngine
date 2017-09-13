#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"

class Application
{
public:

	ModuleWindow*		window = nullptr;
	ModuleInput*		input = nullptr;
	ModuleAudio*		audio = nullptr;
	ModuleRenderer3D*	renderer3D = nullptr;
	ModuleCamera3D*		camera = nullptr;
	ModulePhysics3D*	physics = nullptr;

private:

	Timer			ms_timer;
	float			dt = 0.0f;
	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool			Init();
	update_status	Update();
	bool			CleanUp();

private:

	void AddModule(Module* mod);
	void PrepareUpdate();
	void FinishUpdate();
};