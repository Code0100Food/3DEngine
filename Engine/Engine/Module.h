#ifndef _MODULE_H_
#define _MODULE_H_

#include "Globals.h"
#include <list>
#include <vector>
#include <string>

class Application;
struct PhysBody3D;

using namespace std;

class Module
{
private:

	bool enabled = false;

public:


	Module(bool start_enabled = true)
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

public:

	std::string name = "undef_module";

};

#endif // !_MODULE_H_


