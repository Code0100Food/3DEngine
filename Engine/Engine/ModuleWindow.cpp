#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_sdl.h"
#include "SDL/include/SDL_opengl.h"
#include "FileSystem.h"
#include "Parson/parson.h"

// Constructors =================================
ModuleWindow::ModuleWindow(bool start_enabled) : Module(start_enabled)
{
	name = "module_window";
	window = NULL;
	screen_surface = NULL;
}

// Destructors ==================================
ModuleWindow::~ModuleWindow()
{

}

// Game Loop ====================================
bool ModuleWindow::Awake(const JSON_Object * data_root)
{
	brightness = json_object_get_number(data_root, "brightness");
	width = json_object_get_number(data_root, "width");
	height = json_object_get_number(data_root, "height");

	return true;
}

// Called before render is available
bool ModuleWindow::Init()
{
	LOG("Init SDL window & surface");
	bool ret = true;

	if(SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		LOG("[error] SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	else
	{
		//Create window
		int w_width = width * SCREEN_SIZE;
		int w_height = height * SCREEN_SIZE;
		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(WIN_FULLSCREEN == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(WIN_RESIZABLE == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(WIN_BORDERLESS == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(WIN_FULLSCREEN_DESKTOP == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		}

		window = SDL_CreateWindow(App->app_name.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w_width, w_height, flags);

		if(window == NULL)
		{
			LOG("[error] Window could not be created! SDL_Error: %s\n", SDL_GetError());
			ret = false;
		}
		else
		{
			//Get window surface
			screen_surface = SDL_GetWindowSurface(window);

			//Setup ImGui binding
			ImGui_ImplSdl_Init(window);
		}
	}

	return ret;
}

// Called before quitting
bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	//Destroy window
	if(window != NULL)
	{
		SDL_DestroyWindow(window);
	}

	//Quit SDL subsystems
	SDL_Quit();
	return true;
}

void ModuleWindow::BlitConfigInfo()
{
	//Brightness slice bar
	ImGui::SliderFloat("brightness", &brightness, 0.0f, 1.0f);
	ImGui::SliderInt("width", &width, 0, SCREEN_WIDTH);
	ImGui::SliderInt("height", &height, 0, SCREEN_HEIGHT);

	//Apply button
	if (ImGui::Button("Apply##1", ImVec2(50, 20)))
	{
		//Apply brightness
		GLfloat LightModelAmbient[] = { brightness, brightness, brightness, brightness };
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		//Save values
		//Load config json file
		const JSON_Value *config_data = App->fs->LoadJSONFile("config.json");
		assert(config_data != NULL);
		//Save the new variable
		json_object_set_number(App->fs->AccessObject(config_data, 1, name.c_str()), "brightness", brightness);
		json_object_set_number(App->fs->AccessObject(config_data, 1, name.c_str()), "width", width);
		json_object_set_number(App->fs->AccessObject(config_data, 1, name.c_str()), "height", height);
		//Save the file
		App->fs->SaveJSONFile(config_data, "config.json");

	}
}

// Functionality ================================


// Set Methods ==================================
void ModuleWindow::SetTitle(const char* title)
{
	SDL_SetWindowTitle(window, title);
}

// Get Methods ==================================
float ModuleWindow::GetBrightness() const
{
	return brightness;
}
