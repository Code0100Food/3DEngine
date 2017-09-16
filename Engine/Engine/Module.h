#pragma once

#include "Globals.h"

#include <string>

class Application;
struct PhysBody3D;

class Module
{
public:

	Module(Application* parent, bool start_enabled = true) : App(parent)
	{
	
	}

	virtual ~Module()
	{
	
	}

public:

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


	virtual void OnCollision(PhysBody3D* body1, PhysBody3D* body2) {


	}

protected:

	std::string name = "undef";

private:

	bool enabled = true;

public:

	Application* App = nullptr;

};