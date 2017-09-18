#include "ModuleConsole.h"


// Constructors =================================
ModuleConsole::ModuleConsole(Application * parent, bool start_enabled) :Module(parent, start_enabled)
{
	name = "module_console";
}


// Destructors ==================================
ModuleConsole::~ModuleConsole()
{

}

// Game Loop ====================================
bool ModuleConsole::Start()
{
	return false;
}

update_status ModuleConsole::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}
