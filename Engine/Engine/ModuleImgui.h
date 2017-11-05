#ifndef _ModuleImgui_H_
#define _ModuleImgui_H_

#include "Module.h"
#include "SDL\include\SDL_version.h"

class DockContext;

class ModuleImgui : public Module
{
public:

	ModuleImgui(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleImgui();

public:

	bool			Awake(const JSON_Object* data_root)final;
	bool			Start() final;
	update_status	PreUpdate(float dt) final;
	update_status	Update(float dt) final;
	update_status	PostUpdate(float dt) final;
	bool			CleanUp();
	void			SaveConfigInfo(JSON_Object* data_root)final;

private:

	//Bar menus
	bool	file_menu_open = false;
	bool	gameobjects_open = false;
	bool	view_menu_open = false;
	bool	tools_menu_open = false;
	bool	help_menu_open = false;

	//Window menus
	bool	show_profiler_window = false;
	bool	show_test_window = false;
	bool	show_about_window = false;
	bool	show_ui_conf_window = false;
	bool	show_exit_window = false;

	//UI Configuration window data
	bool		dark_theme = false;
	bool		light_theme = false;
	bool		custom_theme = false;
	ImGuiStyle	custom_style;
	
	void	BlitUIConfigWindow();
	void	BlitAboutWindow();
	void	BlitExitWindow();

	void	SetDarkTheme();
	void	SetLightTheme();
	void	SetCustomTheme();

	//To know sdl version
	SDL_version sdl_version;

	//Father Dock
	DockContext*	father_dock = nullptr;

public:

	void RenderUI();
	void CallExitWindow();

	DockContext* GetWorkspace() const;

};
#endif // _ModuleImgui_H_
