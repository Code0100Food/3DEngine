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
	//Console UI open/close state
	if (open)
	{
		//Generate Console Window
		ImGui::SetNextWindowPos(ImVec2(400, 0));
		ImGui::Begin("c", &open, ImVec2(400, 100), 0.0f, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);

		//Generate Console Input Box
		ImGui::InputText("Input", (char*)console_buffer.c_str(), strlen(console_buffer.c_str()));

		ImGui::End();
	}

	return update_status::UPDATE_CONTINUE;
}

void ModuleConsole::SwapConsoleState()
{
	open = !open;
}
