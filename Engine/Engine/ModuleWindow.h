#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(bool start_enabled = true);
	~ModuleWindow();

public:

	bool Awake(const JSON_Object* data_root)final;
	bool Init() final;
	bool CleanUp() final;
	void BlitConfigInfo()final;
	void SaveConfigInfo(JSON_Object* data_root)final;

private:

	float	brightness = 1.0f;
	int		width = 0;
	int		height = 0;

	bool	full_screen = false;
	bool	resizable = false;
	bool	borderless = false;
	bool	full_desktop = false;

public:

	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

public:
	
	//Set Methods =====================
	void SetTitle(const char* title);

	//Get Methods =====================
	float GetBrightness()const;

};

#endif // __ModuleWindow_H__