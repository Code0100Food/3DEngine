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
#include "FileSystem.h"
#include "ModuleHardware.h"
#include "InputManager.h"
#include "Scene.h"
#include "FileSystem.h"
#include "Profiler.h"
#include "Parson/parson.h"
#include "imgui/imgui_impl_sdl.h"
#include "mmgr/mmgr.h"

// Constructors =================================
Application::Application()
{
	START(ms_timer);
	START(prof_timer);

	profiler = new Profiler();
	
	START(m_prof_timer);
	window = new ModuleWindow();
	profiler->CallProfBlock(M_WINDOW, BUILD_STEP, prof_timer.ReadTicks());
	
	START(m_prof_timer);
	fs = new FileSystem();
	profiler->CallProfBlock(M_FILE_SYSTEM, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	input = new ModuleInput();
	profiler->CallProfBlock(M_INPUT, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	input_manager = new InputManager();
	profiler->CallProfBlock(M_INPUT_MANAGER, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	audio = new ModuleAudio();
	profiler->CallProfBlock(M_AUDIO, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	renderer3D = new ModuleRenderer3D();
	profiler->CallProfBlock(M_RENDERER, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	camera = new ModuleCamera3D();
	profiler->CallProfBlock(M_CAMERA3D, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	physics = new ModulePhysics3D();
	profiler->CallProfBlock(M_PHYSICS3D, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	imgui = new ModuleImgui();
	profiler->CallProfBlock(M_IMGUI, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	console = new ModuleConsole();
	profiler->CallProfBlock(M_CONSOLE, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	hard = new ModuleHardware();
	profiler->CallProfBlock(M_HARDWARE, BUILD_STEP, prof_timer.ReadTicks());

	START(m_prof_timer);
	scene = new Scene();
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
	AddModule(physics);
	AddModule(console);
	AddModule(hard);
	AddModule(profiler);

	// Scenes
	AddModule(scene);

	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);

	PEEK(ms_timer);
	profiler->CallProfBlock(APPLICATION, BUILD_STEP, prof_timer.ReadTicks());

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
	START(ms_timer);
	START(prof_timer);

	bool ret = true;

	//Load config json file
	const JSON_Value *config_data = json_parse_file("config.json");
	assert(config_data != NULL);
	const JSON_Object *root_object = json_value_get_object(config_data);
	
	//Load data from config application child
	const JSON_Object* app_object = json_object_dotget_object(root_object, "Application");
	app_name = json_object_get_string(app_object, "name");
	organization = json_object_get_string(app_object, "organization");
	max_fps = json_object_get_number(app_object, "max_fps");
	capped_ms = 1000 / (float)max_fps;

	// Call Awake() in all modules
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		START(m_prof_timer);
		const JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->Awake(module_object);
		profiler->CallProfBlock((*item)->id, BUILD_STEP, prof_timer.ReadTicks());
	}

	json_value_free((JSON_Value *)config_data);

	PEEK(ms_timer);
	profiler->CallProfBlock(APPLICATION, AWAKE_STEP, prof_timer.ReadTicks());

	return ret;
}

bool Application::Init()
{
	START(ms_timer);
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
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->Start();
		profiler->CallProfBlock((*item)->id, START_STEP, prof_timer.ReadTicks());
	}

	//Initialize values
	memset(fps_array, 0, 30);

	PEEK(ms_timer);
	profiler->CallProfBlock(APPLICATION, START_STEP, prof_timer.ReadTicks());

	return ret;
}

void Application::PrepareUpdate()
{
	frame_count++;
	last_sec_frame_count++;

	dt = (float)ms_timer.ReadSec();
	ms_timer.Start();
	frame_time.Start();

	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);
}

void Application::FinishUpdate()
{
	if (last_sec_frame_time.Read() > 1000)
	{
		last_sec_frame_time.Start();
		prev_last_sec_frame_count = last_sec_frame_count;
		last_sec_frame_count = 0;
	}

	float avg_fps = float(frame_count) / startup_time.ReadSec();
	float seconds_since_startup = startup_time.ReadSec();
	uint32 last_frame_ms = frame_time.Read();
	uint32 frames_on_last_update = prev_last_sec_frame_count;

	if (capped_ms > 0 && last_frame_ms < capped_ms)
	{
		SDL_Delay(capped_ms - last_frame_ms);
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
		ret = (*item)->PreUpdate(dt);
		profiler->CallProfBlock((*item)->id, PRE_UPDATE_STEP, prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, PRE_UPDATE_STEP, prof_timer.ReadTicks());

	START(prof_timer);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->Update(dt);
		profiler->CallProfBlock((*item)->id, UPDATE_STEP, prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, UPDATE_STEP, prof_timer.ReadTicks());

	START(prof_timer);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		if (!(*item)->enabled)continue;

		START(m_prof_timer);
		ret = (*item)->PostUpdate(dt);
		profiler->CallProfBlock((*item)->id, POST_UPDATE_STEP, prof_timer.ReadTicks());
	}
	profiler->CallProfBlock(APPLICATION, POST_UPDATE_STEP, prof_timer.ReadTicks());

	FinishUpdate();

	if (want_to_quit)ret = update_status::UPDATE_STOP;


	return ret;
}

bool Application::CleanUp()
{
	START(ms_timer);

	bool ret = true;

	//Config autosave ---------------------------
	//Save config json file
	const JSON_Value *config_data = App->fs->LoadJSONFile("config.json");
	assert(config_data != NULL);

	//Save the new variable
	JSON_Object * app_value = fs->AccessObject(config_data, 1, "Application");
	json_object_set_string(app_value, "name", app_name.c_str());
	json_object_set_string(app_value, "organization", organization.c_str());
	json_object_set_number(app_value, "max_fps", max_fps);

	// Call Awake() in all modules
	const JSON_Object *root_object = json_value_get_object(config_data);
	for (std::list<Module*>::iterator item = list_modules.begin(); item != list_modules.end(); item++)
	{
		JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->SaveConfigInfo(module_object);
	}

	ret = fs->SaveJSONFile(config_data, "config.json");
	json_value_free((JSON_Value *)config_data);


	//CleanUp -----------------------------------
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		(*item)->CleanUp();
	}

	PEEK(ms_timer);

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
	ImGui::SetNextWindowPos(ImVec2(850, 100));
	ImGui::SetNextWindowSize(ImVec2(420, 600));
	ImGui::Begin("Configuration", &show_config_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove);
	
	//Build application header
	bool cpy = config_opened;
	if (ImGui::CollapsingHeader("Application"))
	{
		//Play fx on first open
		config_opened = true;
		if (cpy != config_opened)App->audio->PlayFxForInput(WINDOW_FX);

		if (ImGui::InputText("Title", (char*)app_name.c_str(), 20, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue))
		{
			App->audio->PlayFxForInput(FX_ID::APPLY_FX);
			App->window->SetTitle(app_name.c_str());
		}
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application title.");

		ImGui::InputText("Organization", (char*)organization.c_str(), 20);
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application organization.");

		if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
		{
			capped_ms = 1000 / (float)max_fps;
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
		ImGui::PlotHistogram("", fps_array, IM_ARRAYSIZE(fps_array), 30, fps_title, 0.0f,130.0f, ImVec2(0, 80));
		
		//Update framerate graphic
		for (uint k = 0; k < GRAPH_ARRAY_SIZE; k++)
		{
			miliseconds_array[k] = miliseconds_array[k + 1];
		}
		miliseconds_array[GRAPH_ARRAY_SIZE - 1] = dt * 1000;
		
		//Blit milliseconds graphic
		char mili_title[25];
		sprintf_s(mili_title, 25, "Milliseconds %.1f", miliseconds_array[29]);
		ImGui::PlotHistogram("", miliseconds_array, IM_ARRAYSIZE(miliseconds_array), 30, mili_title, 0.0f, 100.0f, ImVec2(0, 80));

		ImGui::Separator();

		//Blit memory data
		sMStats memory_stats = 	m_getMemoryStatistics();
		//Total reported memory
		char total_rep_mem[35];
		sprintf_s(total_rep_mem, 35, "Total Reported Mem: %i", memory_stats.totalReportedMemory);
		ImGui::Text("%s", total_rep_mem);

		//Total actual memory
		char total_actual_mem[35];
		sprintf_s(total_actual_mem, 35, "Total Actual Mem: %i", memory_stats.totalActualMemory);
		ImGui::Text("%s", total_actual_mem);

		//Peak reported memory
		char peak_reported_mem[35];
		sprintf_s(peak_reported_mem, 35, "Peak Reported Mem: %i", memory_stats.peakReportedMemory);
		ImGui::Text("%s", peak_reported_mem);

		//Peak actual memory
		char peak_actual_mem[35];
		sprintf_s(peak_actual_mem, 35, "Peak Actual Mem: %i", memory_stats.peakActualMemory);
		ImGui::Text("%s", peak_actual_mem);

		//Accumulated reported memory
		char accumulated_reported_mem[35];
		sprintf_s(accumulated_reported_mem, 35, "Accumulated Reported Mem: %i", memory_stats.accumulatedReportedMemory);
		ImGui::Text("%s", accumulated_reported_mem);

		//Accumulated reported memory
		char accumulated_actual_mem[35];
		sprintf_s(accumulated_actual_mem, 35, "Accumulated Actual Mem: %i", memory_stats.accumulatedActualMemory);
		ImGui::Text("%s", accumulated_actual_mem);

		//Accumulated alloc unit count
		char accumulated_alloc_unit_count[35];
		sprintf_s(accumulated_alloc_unit_count, 35, "Accumulated Alloc Unit Count: %i", memory_stats.accumulatedAllocUnitCount);
		ImGui::Text("%s", accumulated_alloc_unit_count);

		//Total alloc unit count
		char total_alloc_unit_count[35];
		sprintf_s(total_alloc_unit_count, 35, "Total Alloc Unit Count: %i", memory_stats.totalAllocUnitCount);
		ImGui::Text("%s", total_alloc_unit_count);

		//Peak alloc unit count
		char peak_alloc_unit_count[35];
		sprintf_s(peak_alloc_unit_count, 35, "Peak Alloc Unit Count: %i", memory_stats.peakAllocUnitCount);
		ImGui::Text("%s", peak_alloc_unit_count);
	}

	//Play fx when is closed
	else if (config_opened)
	{
		config_opened = false;
		App->audio->PlayFxForInput(WINDOW_FX);
	}

	//Build headers for the rest of modules
	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		if (!(*item)->config_menu)continue;

		bool cpy = (*item)->config_open;
		
		//Play fx on first open
		if (ImGui::CollapsingHeader((*item)->name.c_str(),ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			(*item)->config_open = true;
			if (cpy != (*item)->config_open)App->audio->PlayFxForInput(WINDOW_FX);

			(*item)->BlitConfigInfo();
		}
		//Play fx when is closed
		else if ((*item)->config_open)
		{
			App->audio->PlayFxForInput(WINDOW_FX);
			(*item)->config_open = false;
		}
	}
	
	ImGui::End();
}

void Application::AddModule(Module* mod)
{
	list_modules.push_back(mod);
}

