#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "SDL/include/SDL.h"

class Application;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);
	~ModuleWindow();

public:

	bool Init() final;
	bool CleanUp() final;
	
public:

	//The window we'll be rendering to
	SDL_Window* window = nullptr;

	//The surface contained by the window
	SDL_Surface* screen_surface = nullptr;

public:
	
	void SetTitle(const char* title);
};

#endif // __ModuleWindow_H__