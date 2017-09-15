#ifndef _ModuleImgui_H_
#define _ModuleImgui_H_

#include "Module.h"
#include "imgui/imgui.h"

class ModuleImgui : public Module
{
public:

	ModuleImgui(Application* parent, bool start_enabled = true) : Module(parent, start_enabled) {}

public:

	bool Start();
	update_status Update(float dt);
	update_status PostUpdate(float dt);

private:

	bool	show_test_window = false;
	bool	show_another_window = false;
	ImVec4	clear_color = ImColor(255, 255, 255);

};
#endif // !_ModuleImgui_H_
