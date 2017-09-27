#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "imgui/imgui_impl_sdl.h"
#include "SDL/include/SDL_opengl.h"
#include "FileSystem.h"
#include "ModuleAudio.h"

// Constructors =================================
ModuleWindow::ModuleWindow(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{
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
	full_screen = json_object_get_boolean(data_root, "full_screen");
	resizable = json_object_get_boolean(data_root, "resizable");
	borderless = json_object_get_boolean(data_root, "borderless");
	full_desktop = json_object_get_boolean(data_root, "full_desktop");

	config_menu = true;

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


		//Set Atributes
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
		SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

		//Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		if(full_screen == true)
		{
			flags |= SDL_WINDOW_FULLSCREEN;
		}

		if(resizable == true)
		{
			flags |= SDL_WINDOW_RESIZABLE;
		}

		if(borderless == true)
		{
			flags |= SDL_WINDOW_BORDERLESS;
		}

		if(full_desktop == true)
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

			//Apply brightness
			SDL_SetWindowBrightness(window, brightness);
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
	if (ImGui::SliderFloat("Brightness", &brightness, 0.0f, 1.0f))
	{
		//Apply brightness
		SDL_SetWindowBrightness(window, brightness);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change the global light brightness.");

	//Width slice bar
	if (ImGui::SliderInt("Width", &width, 0, SCREEN_WIDTH))
	{
		SDL_SetWindowSize(window, width, height);
		App->audio->PlayFxForInput(SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change the window width.");

	//Height slice bar
	if (ImGui::SliderInt("Height", &height, 0, SCREEN_HEIGHT))
	{
		SDL_SetWindowSize(window, width, height);
		App->audio->PlayFxForInput(SLICE_TICK_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change the window height.");

	//Framerate 
	ImGui::Text("Framerate: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%.1f", ImGui::GetIO().Framerate);
	
	ImGui::Separator();

	//Full Screen checkbox
	if (ImGui::Checkbox("Full Screen", &full_screen))
	{
		SDL_SetWindowFullscreen(window, full_screen ? SDL_WINDOW_FULLSCREEN : 0);
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF fullscreen mode.");
	ImGui::SameLine();

	//Resizable checkbox
	if (ImGui::Checkbox("Resizable", &resizable))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF window resize [NEED RESTART]");

	//Borderless checkbox
	if(ImGui::Checkbox("Borderless ", &borderless))
	{
		SDL_SetWindowBordered(window, (SDL_bool)!borderless);
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Show/Hide window borders.");
	ImGui::SameLine();

	//Full Desktop checkbox
	if(ImGui::Checkbox("Full Desktop", &full_desktop))
	{
		SDL_SetWindowFullscreen(window, full_desktop ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF window full desktop mode.");
}

void ModuleWindow::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_number(data_root, "brightness", brightness);
	json_object_set_number(data_root, "width", width);
	json_object_set_number(data_root, "height", height);
	json_object_set_boolean(data_root, "full_screen", full_screen);
	json_object_set_boolean(data_root, "resizable", resizable);
	json_object_set_boolean(data_root, "borderless", borderless);
	json_object_set_boolean(data_root, "full_desktop", full_desktop);
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
