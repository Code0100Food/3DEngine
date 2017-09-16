#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Timer.h"

class Module;
class ModuleWindow;
class ModuleInput;
class ModuleAudio;
class ModuleRenderer3D;
class ModuleCamera3D;
class ModulePhysics3D;
class ModuleImgui;

class Application
{
public:

	Application();
	~Application();

public:

	bool			Init();
	void			PrepareUpdate();
	update_status	Update();
	void			FinishUpdate();
	bool			CleanUp();

public:

	ModuleWindow*		window = nullptr;
	ModuleInput*		input = nullptr;
	ModuleAudio*		audio = nullptr;
	ModuleRenderer3D*	renderer3D = nullptr;
	ModuleCamera3D*		camera = nullptr;
	ModulePhysics3D*	physics = nullptr;
	ModuleImgui*		imgui = nullptr;

private:

	Timer			ms_timer;
	float			dt = 0.0f;
	p2List<Module*> list_modules;
	bool			want_to_quit = false;

public:

	void SetQuit();

private:

	void AddModule(Module* mod);

};