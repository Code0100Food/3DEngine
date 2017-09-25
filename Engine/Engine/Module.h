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

class Module
{
public:

	bool enabled = false;

public:


	// Constructors =================================
	Module(bool start_enabled = true) :enabled(start_enabled)
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

public:

	std::string name = "undef_module";
	bool		config_menu = false;
	bool		config_open = false;

};

#endif // !_MODULE_H_


