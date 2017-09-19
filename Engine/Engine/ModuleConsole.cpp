#include "ModuleConsole.h"
#include "imgui/imgui.h"

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
	
	return true;
}

update_status ModuleConsole::Update(float dt)
{
	bool open = true;
	ShowExampleAppConsole(&open);

	return update_status::UPDATE_CONTINUE;
}

void ModuleConsole::SwapConsoleState()
{
	open = !open;
}
