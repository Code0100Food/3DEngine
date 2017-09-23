#ifndef _ModuleImgui_H_
#define _ModuleImgui_H_

#include "Module.h"
#include "SDL\include\SDL_version.h"

class ModuleImgui : public Module
{
public:

	ModuleImgui(bool start_enabled = true);

public:

	bool			Start() final;
	update_status	Update(float dt) final;
	update_status	PostUpdate(float dt) final;

private:

	bool	show_test_window = false;
	bool	show_about_window = false;

	bool	debug_text = false;
	ImVec4	clear_color = ImColor(255, 255, 255);

	void	BlitAboutWindow();
	void	ShowAbout();

	//To know sdl version
	SDL_version sdl_version;

};
#endif // _ModuleImgui_H_
