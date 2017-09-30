#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"
#include "Parson/parson.h"
#include "imgui/imgui.h"
#include <list>
#include <vector>
#include <string>

class Application;
struct PhysBody3D;

using namespace std;

enum MODULE_ID
{
	UNDEF_MODULE = 0,
	APPLICATION,
	M_CONSOLE,
	M_FILE_SYSTEM,
	M_AUDIO,
	M_CAMERA3D,
	M_HARDWARE,
	M_IMGUI,
	M_INPUT,
	M_INPUT_MANAGER,
	M_PHYSICS3D,
	M_RENDERER,
	M_WINDOW,
	M_GEOMETRY,
	M_PROFILER,
	M_SCENE
};

class Module
{
public:

	// Constructors =================================
	Module(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true) : name(_name), id(_id), config_menu(_config_menu), enabled(_enabled)
	{

	}

	// Destructors ==================================
	virtual ~Module()
	{
	
	}

public:

	// Game Loop ====================================
	virtual bool Awake(const JSON_Object* data_root)
	{
		return true;
	}

	virtual bool Init()
	{
		return true;
	}

	virtual bool Start()
	{
		return true;
	}

	virtual update_status PreUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status Update(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual update_status PostUpdate(float dt)
	{
		return UPDATE_CONTINUE;
	}

	virtual bool CleanUp()
	{
		return true;
	}

	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2) 
	{

	}

	virtual void BlitConfigInfo()
	{

	}

	virtual void SaveConfigInfo(JSON_Object* data_root)
	{

	}

public:

	bool		enabled = false;
	MODULE_ID	id = UNDEF_MODULE;
	std::string name = "undef_module";
	bool		config_menu = false;
	bool		config_open = false;

};

#endif // !_MODULE_H_


