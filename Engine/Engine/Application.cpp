#include "Application.h"
#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModuleImgui.h"
#include "ModuleConsole.h"
#include "FileSystem.h"
#include "ModuleHardware.h"
#include "InputManager.h"
#include "GeometryManager.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "FileSystem.h"
#include "Profiler.h"
#include "Serializer.h"
#include "ImporterManager.h"
#include "TimeManager.h"
#include "ResourcesManager.h"
#include "Parson/parson.h"
#include "imgui/imgui_impl_sdl.h"
#include "mmgr/mmgr.h"
#include "imgui/imgui_dock.h"

// Constructors =================================
Application::Application()
{
	START(prof_timer);

	importer = new ImporterManager();

	profiler = new Profiler();
	
	serializer = new Serializer();

	randomizer = new math::LCG();

	time_manager = new TimeManager();

	START(m_prof_timer);
	window = new ModuleWindow("Window", M_WINDOW, true);
	profiler->CallProfBlock(M_WINDOW, BUILD_STEP, prof_timer.ReadTicks());
	
	START(m_prof_timer);
	fs = new FileSystem("FileSystem", M_FILE_SYSTEM, false);
	profiler->CallProfBlock(M_FILE_SYSTEM, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	input = new ModuleInput("Input", M_INPUT, true);
	profiler->CallProfBlock(M_INPUT, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	input_manager = new InputManager("InputManager", M_INPUT_MANAGER, true);
	profiler->CallProfBlock(M_INPUT_MANAGER, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	audio = new ModuleAudio("Audio", M_AUDIO, true);
	profiler->CallProfBlock(M_AUDIO, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	renderer3D = new ModuleRenderer3D("Renderer", M_RENDERER, true);
	profiler->CallProfBlock(M_RENDERER, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	camera = new ModuleCamera3D("Camera", M_CAMERA3D, true);
	profiler->CallProfBlock(M_CAMERA3D, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	imgui = new ModuleImgui("ImGui", M_IMGUI, false);
	profiler->CallProfBlock(M_IMGUI, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	console = new ModuleConsole("Console", M_CONSOLE, true);
	profiler->CallProfBlock(M_CONSOLE, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	hard = new ModuleHardware("Hardware", M_HARDWARE, true);;
	profiler->CallProfBlock(M_HARDWARE, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	textures = new ModuleTextures("Textures", M_TEXTURES, true);;
	profiler->CallProfBlock(M_TEXTURES, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	geometry = new GeometryManager("Geometry Manager", M_GEOMETRY, true);
	profiler->CallProfBlock(M_GEOMETRY, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	res_manager = new ResourcesManager("Resources Manager", M_RES_MANAGER, true);
	profiler->CallProfBlock(M_RES_MANAGER, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	scene = new ModuleScene("Scene", M_SCENE, true);
	profiler->CallProfBlock(M_SCENE, BUILD_STEP, prof_timer.ReadTicks());

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(imgui);
	AddModule(fs);
	AddModule(window);
	AddModule(input);
	AddModule(input_manager);
	AddModule(audio);
	AddModule(console);
	AddModule(hard);
	AddModule(textures);
	AddModule(geometry);
	AddModule(res_manager);

	// Scenes
	AddModule(scene);

	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);

	profiler->CallProfBlock(APPLICATION, BUILD_STEP, prof_timer.ReadTicks());

	//Dock Window
	config_dock = new DockContext();

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
	START(App->time_manager->ms_timer);
	START(prof_timer);

	bool ret = true;

	//Load config json file
	char str[50];
	sprintf(str, "%sconfig.json", SETTINGS_FOLDER);
	const JSON_Value *config_data = json_parse_file(str);
	assert(config_data != NULL);
	const JSON_Object *root_object = json_value_get_object(config_data);
	
	//Load data from config application child
	const JSON_Object* app_object = json_object_dotget_object(root_object, "Application");
	app_name = json_object_get_string(app_object, "name");
	organization = json_object_get_string(app_object, "organization");
	App->time_manager->max_fps = json_object_get_number(app_object, "max_fps");
	App->time_manager->capped_ms = 1000 / (float)App->time_manager->max_fps;

	// Call Awake() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		START(m_prof_timer);
		const JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->Awake(module_object);
		profiler->CallProfBlock((*item)->id, BUILD_STEP, prof_timer.ReadTicks());
	}

	//Load profiler configuration
	const JSON_Object* profiler_object = json_object_dotget_object(root_object, "Profiler");
	profiler->LoadConfiguration(profiler_object);

	//Free config data
	json_value_free((JSON_Value *)config_data);

	PEEK(App->time_manager->ms_timer);
	profiler->CallProfBlock(APPLICATION, AWAKE_STEP, prof_timer.ReadTicks());

	return ret;
}

bool Application::Init()
{
	START(App->time_manager->ms_timer);
	START(prof_timer);

	bool ret = true;

	// Call Init() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		START(m_prof_timer);
		ret = (*item)->Init();
		profiler->CallProfBlock((*item)->id, INIT_STEP, prof_timer.ReadTicks());
	}

	// After all Init calls we call Start() in all modules
	LOG("Application Start --------------");
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end() && ret; item++)
	{
		START(m_prof_timer);
		ret = (*item)->Start();
		profiler->CallProfBlock((*item)->id, START_STEP, prof_timer.ReadTicks());
	}

	//Initialize values
	memset(fps_array, 0, 30);
	app_context = APP_CONTEXT::PAUSE_CONTEXT;
	PEEK(App->time_manager->ms_timer);
	profiler->CallProfBlock(APPLICATION, START_STEP, prof_timer.ReadTicks());

	return ret;
}

void Application::PrepareUpdate()
{
	App->time_manager->frame_count++;
	App->time_manager->last_sec_frame_count++;

	App->time_manager->real_time_dt = (float)App->time_manager->ms_timer.ReadSec();
	App->time_manager->ms_timer.Start();
	App->time_manager->frame_time.Start();

	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);
}

void Application::FinishUpdate()
{
	if (App->time_manager->last_sec_frame_time.Read() > 1000)
	{
		App->time_manager->last_sec_frame_time.Start();
		App->time_manager->prev_last_sec_frame_count = App->time_manager->last_sec_frame_count;
		App->time_manager->last_sec_frame_count = 0;
	}

	float avg_fps = float(App->time_manager->frame_count) / App->time_manager->real_startup_time.ReadSec();
	float seconds_since_startup = App->time_manager->real_startup_time.ReadSec();
	uint32 last_frame_ms = App->time_manager->frame_time.Read();
	
	if (App->time_manager->capped_ms > 0 && last_frame_ms < App->time_manager->capped_ms)
	{
		SDL_Delay(App->time_manager->capped_ms - last_frame_ms);
	}
}

// Call PreUpdate, Update and PostUpdate on all modules
update_status Application::Update()
{

	update_status ret = UPDATE_CONTINUE;

	//Start frame timer & ImGui new frame
	PrepareUpdate();
	
	START(prof_timer);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->PreUpdate(App->time_manager->real_time_dt);
		profiler->CallProfBlock((*item)->id, PRE_UPDATE_STEP, m_prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, PRE_UPDATE_STEP, prof_timer.ReadTicks());

	START(prof_timer);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->Update(App->time_manager->real_time_dt);
		profiler->CallProfBlock((*item)->id, UPDATE_STEP, m_prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, UPDATE_STEP, prof_timer.ReadTicks());

	START(prof_timer);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->PostUpdate(App->time_manager->real_time_dt);
		profiler->CallProfBlock((*item)->id, POST_UPDATE_STEP, m_prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, POST_UPDATE_STEP, prof_timer.ReadTicks());

	FinishUpdate();

	if (want_to_quit)ret = update_status::UPDATE_STOP;


	return ret;
}

bool Application::CleanUp()
{
	START(App->time_manager->ms_timer);

	bool ret = true;

	//Config autosave ---------------------------
	//Save config json file
	char str[50];
	sprintf(str, "%sconfig.json", SETTINGS_FOLDER);
	const JSON_Value *config_data = App->fs->LoadJSONFile(str);
	assert(config_data != NULL);

	//Save the new variable
	JSON_Object * app_value = serializer->AccessObject(config_data, 1, "Application");
	json_object_set_string(app_value, "name", app_name.c_str());
	json_object_set_string(app_value, "organization", organization.c_str());
	json_object_set_number(app_value, "max_fps", App->time_manager->max_fps);

	// Save all modules configuration
	const JSON_Object *root_object = json_value_get_object(config_data);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->SaveConfigInfo(module_object);
	}

	//Save profiler configuration
	profiler->SaveConfiguration(json_object_dotget_object(root_object, "Profiler"));
	sprintf(str, "%sconfig.json", SETTINGS_FOLDER);
	ret = fs->SaveJSONFile(config_data, str);
	json_value_free((JSON_Value *)config_data);


	//CleanUp -----------------------------------
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		(*item)->CleanUp();
	}

	//Delete the profiler
	RELEASE(profiler);

	//Delete the importer
	RELEASE(importer);

	//Delete the serializer
	RELEASE(serializer);

	//Delete the randomizer
	RELEASE(randomizer);

	//Release the config dock
	RELEASE(config_dock);

	PEEK(App->time_manager->ms_timer);

	//Delete the time manager
	RELEASE(time_manager);

	return ret;
}

// Functionality ================================
APP_CONTEXT Application::StrToAppContext(const char * str) const
{
	if (strcmp(str, "in_game_context") == 0)		return IN_GAME_CONTEXT;
	if (strcmp(str, "pause_context") == 0)			return PAUSE_CONTEXT;
	if (strcmp(str, "main_menu_context") == 0)		return MAIN_MENU_CONTEXT;
	return UNKNOWN_CONTEXT;
}

const char*	Application::AppContextToStr(APP_CONTEXT app_context) const
{
	switch (app_context)
	{
	case UNKNOWN_CONTEXT:	return "Unknown";	break;
	case BUILD_CONTEXT:		return "Build";		break;
	case PLAY_CONTEXT:		return "Play";		break;
	case IN_GAME_CONTEXT:	return "Game";		break;
	case PAUSE_CONTEXT:		return "Pause";		break;
	}
}

MODULE_ID Application::StrToModuleID(const char * str) const
{
	if (strcmp(str, "Application") == 0)	return APPLICATION;
	if (strcmp(str, "Console") == 0)		return M_CONSOLE;
	if (strcmp(str, "FileSystem") == 0)		return M_FILE_SYSTEM;
	if (strcmp(str, "Audio") == 0)			return M_AUDIO;
	if (strcmp(str, "Camera") == 0)			return M_CAMERA3D;
	if (strcmp(str, "Hardware") == 0)		return M_HARDWARE;
	if (strcmp(str, "ImGui") == 0)			return M_IMGUI;
	if (strcmp(str, "Input") == 0)			return M_INPUT;
	if (strcmp(str, "InputManager") == 0)	return M_INPUT_MANAGER;
	if (strcmp(str, "Renderer") == 0)		return M_RENDERER;
	if (strcmp(str, "Window") == 0)			return M_WINDOW;
	if (strcmp(str, "Scene") == 0)			return M_SCENE;
	return MODULE_ID::UNDEF_MODULE;
}

void Application::SetQuit()
{
	want_to_quit = true;
}

void Application::SetDebugMode()
{
	debug_mode = !debug_mode;
}

bool Application::GetDebugMode() const
{
	return debug_mode;
}

void Application::RequestBrowser(const char* link)
{
	ShellExecute(NULL, "open", link, NULL, NULL, SW_SHOWNORMAL);
}

void Application::ShowConfiguration()
{
	show_config_window = !show_config_window;
}

bool Application::GetConfigWindowState() const
{
	return show_config_window;
}

void Application::BlitConfigWindow()
{

	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.5f, (App->window->GetHeight() - 23) * 0.5f), App->imgui->GetUICondFlag());
	ImGui::SetNextWindowPos(ImVec2(0, 23), App->imgui->GetUICondFlag());

	ImGui::Begin("Config Workspace##window", &show_config_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "Configuration");
	config_dock->BeginWorkspace("Config Workspace");

	//Begin application dock
	bool ApplicationDock = true;
	config_dock->BeginDock("Application", &ApplicationDock, 0);
	{
		if (ImGui::InputText("Title", (char*)app_name.c_str(), 20, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
		{
			App->audio->PlayFxForInput(FX_ID::APPLY_FX);
			App->window->SetTitle(app_name.c_str());
		}
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application title.");

		ImGui::InputText("Organization", (char*)organization.c_str(), 20);
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application organization.");

		if (ImGui::SliderInt("Max FPS", &App->time_manager->max_fps, 0, 120))
		{
			App->time_manager->capped_ms = 1000 / (float)App->time_manager->max_fps;
			App->audio->PlayFxForInput(SLICE_TICK_FX);
		}
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Limit max FPS.");

		ImGui::Separator();

		//Update framerate graphic
		for (uint k = 0; k < GRAPH_ARRAY_SIZE; k++)
		{
			fps_array[k] = fps_array[k + 1];
		}
		fps_array[GRAPH_ARRAY_SIZE - 1] = ImGui::GetIO().Framerate;

		//Blit framerate graphic
		char fps_title[25];
		sprintf_s(fps_title, 25, "Framerate %.1f", fps_array[29]);
		ImGui::PlotHistogram("", fps_array, IM_ARRAYSIZE(fps_array), 30, fps_title, 0.0f, 130.0f, ImVec2(0, 80));

		//Update framerate graphic
		for (uint k = 0; k < GRAPH_ARRAY_SIZE; k++)
		{
			miliseconds_array[k] = miliseconds_array[k + 1];
		}
		miliseconds_array[GRAPH_ARRAY_SIZE - 1] = App->time_manager->real_time_dt * 1000;

		//Blit milliseconds graphic
		char mili_title[25];
		sprintf_s(mili_title, 25, "Milliseconds %.1f", miliseconds_array[29]);
		ImGui::PlotHistogram("", miliseconds_array, IM_ARRAYSIZE(miliseconds_array), 30, mili_title, 0.0f, 100.0f, ImVec2(0, 80));

		ImGui::Separator();

		//Blit memory data
		sMStats memory_stats = m_getMemoryStatistics();
		//Total reported memory
		char total_rep_mem[60];
		sprintf_s(total_rep_mem, 60, "Total Reported Mem: %i", memory_stats.totalReportedMemory);
		ImGui::Text("%s", total_rep_mem);

		//Total actual memory
		char total_actual_mem[60];
		sprintf_s(total_actual_mem, 60, "Total Actual Mem: %i", memory_stats.totalActualMemory);
		ImGui::Text("%s", total_actual_mem);

		//Peak reported memory
		char peak_reported_mem[60];
		sprintf_s(peak_reported_mem, 60, "Peak Reported Mem: %i", memory_stats.peakReportedMemory);
		ImGui::Text("%s", peak_reported_mem);

		//Peak actual memory
		char peak_actual_mem[60];
		sprintf_s(peak_actual_mem, 60, "Peak Actual Mem: %i", memory_stats.peakActualMemory);
		ImGui::Text("%s", peak_actual_mem);

		//Accumulated reported memory
		char accumulated_reported_mem[60];
		sprintf_s(accumulated_reported_mem, 60, "Accumulated Reported Mem: %i", memory_stats.accumulatedReportedMemory);
		ImGui::Text("%s", accumulated_reported_mem);

		//Accumulated reported memory
		char accumulated_actual_mem[60];
		sprintf_s(accumulated_actual_mem, 60, "Accumulated Actual Mem: %i", memory_stats.accumulatedActualMemory);
		ImGui::Text("%s", accumulated_actual_mem);

		//Accumulated alloc unit count
		char accumulated_alloc_unit_count[60];
		sprintf_s(accumulated_alloc_unit_count, 60, "Accumulated Alloc Unit Count: %i", memory_stats.accumulatedAllocUnitCount);
		ImGui::Text("%s", accumulated_alloc_unit_count);

		//Total alloc unit count
		char total_alloc_unit_count[60];
		sprintf_s(total_alloc_unit_count, 60, "Total Alloc Unit Count: %i", memory_stats.totalAllocUnitCount);
		ImGui::Text("%s", total_alloc_unit_count);

		//Peak alloc unit count
		char peak_alloc_unit_count[60];
		sprintf_s(peak_alloc_unit_count, 60, "Peak Alloc Unit Count: %i", memory_stats.peakAllocUnitCount);
		ImGui::Text("%s", peak_alloc_unit_count);

	}

	//Close Dock
	config_dock->EndDock();
	
	//Build headers for the rest of modules
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		if (!(*item)->config_menu)continue;

		bool cpy = true;

		config_dock->BeginDock((*item)->name.c_str(), &cpy, 0);
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), (*item)->name.c_str());
		(*item)->BlitConfigInfo();
		config_dock->EndDock();

	}
	
	config_dock->EndWorkspace();
	ImGui::End();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

