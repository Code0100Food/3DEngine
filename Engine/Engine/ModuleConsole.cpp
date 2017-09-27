#include "ModuleConsole.h"

// Constructors =================================
ModuleConsole::ModuleConsole(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{

}


// Destructors ==================================
ModuleConsole::~ModuleConsole()
{

}

// Game Loop ====================================
bool ModuleConsole::Start()
{
	config_menu = true;

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

void ModuleConsole::BlitConfigInfo()
{
	int size = console.commands_str.size();
	for (uint k = 0;k < size; k++)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f),"%s: ", console.commands_str[k].c_str());
		ImGui::SameLine();
		ImGui::Text("%s", console.commands_desc_str[k].c_str());
		ImGui::Separator();
	}
	ImGui::Text("To open/close the console press ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "GRAVE_KEY");
}

void ModuleConsole::SwapConsoleState()
{
	open = !open;
}

void ModuleConsole::AddLabel(const char * str)
{
	console.AddLog(str);
}
