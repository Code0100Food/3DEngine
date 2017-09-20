#include "ModuleConsole.h"
#include "imgui/imgui.h"

// Constructors =================================
ModuleConsole::ModuleConsole(bool start_enabled) :Module(start_enabled)
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
	
	return true;
}

update_status ModuleConsole::Update(float dt)
{
	if (open)
	{
		console._Draw("Console", &open);
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleConsole::SwapConsoleState()
{
	open = !open;
}
