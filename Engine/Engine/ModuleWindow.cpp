#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRenderer3D.h"
#include "imgui/imgui_impl_sdl.h"
#include "SDL/include/SDL_opengl.h"
#include "FileSystem.h"
#include "ModuleAudio.h"
#include "ModuleImgui.h"

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

	aspect_ratio = width / height;

	//Load window sizes
	const JSON_Object* win = json_object_get_object(data_root, "win0");

	for (int i = 1; win != nullptr; i++)
	{
		//Load Data
		int new_width = json_object_get_number(win, "width");
		int new_height = json_object_get_number(win, "height");

		WindowSize* new_pre_defined_windows = new WindowSize(new_width, new_height);
		if (new_pre_defined_windows)
		{
			pre_defined_windows.push_back(new_pre_defined_windows);
		}

		//Fill the string for the Combo
		char wd[5];
		char ht[5];
		itoa(new_width, wd, 10);
		itoa(new_height, ht, 10);
		string w = wd;
		string h = ht;
		string size = w + "x" + h;
		windows_names.push_back(size);

		//Jump to next window
		char index[5];
		itoa(i, index, 10);
		string number = index;
		string next_win = "win" + number;
		win = json_object_get_object(data_root, next_win.c_str());
	}

	//Set the current pre defined window index
	uint size = pre_defined_windows.size();
	for (uint k = 0; k < size; k++)
	{
		if (pre_defined_windows[k]->width == width && pre_defined_windows[k]->height == height)
		{
			current_window_index = k;
		}
	}

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


		//Set Attributes
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

	for (std::vector<WindowSize*>::iterator item = pre_defined_windows.begin(); item != pre_defined_windows.end(); item++)
	{
		RELEASE((*item));
	}
	pre_defined_windows.clear();

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

	//Window sizes
	const char* sizes[] = {windows_names[0].c_str(), windows_names[1].c_str(), windows_names[2].c_str(), windows_names[3].c_str(), windows_names[4].c_str(), windows_names[5].c_str() };
	
	if (ImGui::Combo("Resolution", &current_window_index, sizes, IM_ARRAYSIZE(sizes)))
	{
		width = pre_defined_windows[current_window_index]->width;
		height = pre_defined_windows[current_window_index]->height;
		SDL_SetWindowSize(window, width, height);
		App->renderer3D->OnResize(width, height);
		App->imgui->SetCondFlag(ImGuiCond_::ImGuiCond_Always);
	}

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

void ModuleWindow::SetAspectRatio(int width, int height)
{
	aspect_ratio = (float)width / height;
}

void ModuleWindow::SetSceneAspectRatio(int width, int height)
{
	scene_aspect_ratio = (float)width / height;
}

void ModuleWindow::SetGameAspectRatio(int width, int height)
{
	game_aspect_ratio = (float)width / height;
}

void ModuleWindow::SetWidth(int width)
{
	this->width = width;
}

void ModuleWindow::SetHeight(int height)
{
	this->height = height;
}

// Get Methods ==================================
float ModuleWindow::GetBrightness() const
{
	return brightness;
}

int ModuleWindow::GetWidth() const
{
	return width;
}

int ModuleWindow::GetHeight() const
{
	return height;
}

float ModuleWindow::GetAspectRatio() const
{
	return aspect_ratio;
}

float ModuleWindow::GetSceneAspectRation() const
{
	return scene_aspect_ratio;
}

float ModuleWindow::GetGameAspectRatio() const
{
	return game_aspect_ratio;
}
