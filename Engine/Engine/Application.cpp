#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"
#include "ModuleImgui.h"
#include "ModuleConsole.h"
#include "Scene.h"
#include "Parson/parson.h"
#include "imgui/imgui_impl_sdl.h"

// Constructors =================================
Application::Application()
{
	window = new ModuleWindow();
	input = new ModuleInput();
	audio = new ModuleAudio();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	physics = new ModulePhysics3D();
	imgui = new ModuleImgui();
	console = new ModuleConsole();

	scene = new Scene();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(imgui);
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(console);

	// Scenes
	AddModule(scene);

	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);
}

// Destructors ==================================
Application::~Application()
{
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		RELEASE(*item);
	}
}

// Game Loop ====================================
bool Application::Awake()
{
	bool ret = true;

	//Load config json file
	const JSON_Value *config_data = json_parse_file("config.json");
	assert(config_data != NULL);
	const JSON_Object *root_object = json_value_get_object(config_data);
	
	//Load data from config application child
	const JSON_Object* app_object = json_object_dotget_object(root_object, "application");
	app_name = json_object_get_string(app_object, "name");
	organization = json_object_get_string(app_object, "organization");

	// Call Awake() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		const JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		ret = (*item)->Awake(module_object);
	}

	return ret;
}

bool Application::Init()
{
	bool ret = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		ret = (*item)->Init();
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		ret = (*item)->Start();
	}

	ms_timer.Start();
	return ret;
}

void Application::PrepareUpdate()
{
	dt = (float)ms_timer.Read() / 1000.0f;
	ms_timer.Start();

	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);
}

void Application::FinishUpdate()
{

}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{

	update_status ret = UPDATE_CONTINUE;

	//Start frame timer & ImGui new frame
	PrepareUpdate();
	
	//Generate Config Window 
	if (show_config_window)BlitConfigWindow();

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->PreUpdate(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->Update(dt);
	}

	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		ret = (*item)->PostUpdate(dt);
	}

	FinishUpdate();

	if (want_to_quit)ret = update_status::UPDATE_STOP;

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		(*item)->CleanUp();
	}

	return ret;
}

// Functionality ================================
void Application::SetQuit()
{
	want_to_quit = true;
}

void Application::RequestBrowser(const char* link)
{
	ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
}

void Application::ShowConfiguration()
{
	show_config_window = !show_config_window;
}

void Application::BlitConfigWindow()
{
	//Build configuration base window
	ImGui::SetNextWindowPos(ImVec2(900, 100));
	ImGui::SetNextWindowSize(ImVec2(350, 600));
	ImGui::Begin("Configuration", &show_config_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
	//Build application header
	if (ImGui::CollapsingHeader("Application"))
	{
		if (ImGui::InputText("Title", (char*)test, 50))
		{
			LOG("ED");
		}
		LOG("%s", app_name.c_str());
	}

	//Build headers for the rest of modules
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		if (ImGui::CollapsingHeader((*item)->name.c_str()))
		{
			(*item)->BlitConfigInfo();
		}
	}
	ImGui::End();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

