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
#include "Scene.h"
#include "FileSystem.h"
#include "Parson/parson.h"
#include "imgui/imgui_impl_sdl.h"
#include "mmgr/mmgr.h"

// Constructors =================================
Application::Application()
{

	PERF_START(ms_timer);

	window = new ModuleWindow();
	fs = new FileSystem();
	input = new ModuleInput();
	audio = new ModuleAudio();
	renderer3D = new ModuleRenderer3D();
	camera = new ModuleCamera3D();
	physics = new ModulePhysics3D();
	imgui = new ModuleImgui();
	console = new ModuleConsole();
	hard = new ModuleHardware();

	scene = new Scene();

	// The order of calls is very important!
	// Modules will Init() Start() and Update in this order
	// They will CleanUp() in reverse order

	// Main Modules
	AddModule(imgui);
	AddModule(fs);
	AddModule(window);
	AddModule(input);
	AddModule(audio);
	AddModule(physics);
	AddModule(console);
	AddModule(hard);

	// Scenes
	AddModule(scene);

	AddModule(camera);

	// Renderer last!
	AddModule(renderer3D);

	PERF_PEEK(ms_timer);
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
	PERF_START(ms_timer);

	bool ret = true;

	//Load config json file
	std::string dir = App->fs->data_folder + "/" + "config.json";
	const JSON_Value *config_data = json_parse_file(dir.c_str());
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
		const JSON_Object* module_object = json_object_dotget_object(root_object, item._Ptr->_Myval->name.c_str());
		(*item)->Awake(module_object);
	}

	json_value_free((JSON_Value *)config_data);

	PERF_PEEK(ms_timer);

	return ret;
}

bool Application::Init()
{
	PERF_START(ms_timer);

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

	//Initialize values
	memset(fps_array, 0, 30);

	PERF_PEEK(ms_timer);
	ms_timer.Start();	

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
	PERF_START(ms_timer);

	bool ret = true;

	for (std::list<Module*>::reverse_iterator item = list_modules.rbegin(); item != list_modules.rend(); item++)
	{
		(*item)->CleanUp();
	}

	PERF_PEEK(ms_timer);

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

		ImGui::InputText("Title", (char*)app_name.c_str(), 20, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue);
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application title.");
		ImGui::InputText("Organization", (char*)organization.c_str(), 20);
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Change application organization.");

		if (ImGui::SliderInt("Max FPS", &max_fps, 0, 120))
		{
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

		ImGui::Separator();

		if (ImGui::Button("Apply##1", ImVec2(50, 20)))
		{
			//Load config json file
			const JSON_Value *config_data = fs->LoadJSONFile("config.json");
			assert(config_data != NULL);
			//Save the new variable
			JSON_Object * app_value = fs->AccessObject(config_data, 1, "application");
			json_object_set_string(app_value, "name", app_name.c_str());
			json_object_set_string(app_value, "organization", organization.c_str());
			json_object_set_number(app_value, "max_fps", max_fps);
			//Save the file
			fs->SaveJSONFile(config_data, "config.json");
			json_value_free((JSON_Value*)config_data);
			//Update window title
			App->window->SetTitle(app_name.c_str());
			capped_ms = 1000 / (float)max_fps;

			//Play save fx
			App->audio->PlayFxForInput(FX_ID::APPLY_FX);
		}
		ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Press Apply to save all the changes.");
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
		if (ImGui::CollapsingHeader((*item)->name.c_str(),ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_NoTreePushOnOpen))
		{
			//Play fx on first open
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

