#include "ModuleHardware.h"
#include <gl/GL.h>
#include <gl/GLU.h>

// Constructors =================================
ModuleHardware::ModuleHardware(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) :Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleHardware::~ModuleHardware()
{

}


// Game Loop ====================================
bool ModuleHardware::Start()
{
	//CPU AND RAM
	num_cpus = SDL_GetCPUCount();
	cpu_cache = SDL_GetCPUCacheLineSize();
	system_ram = SDL_GetSystemRAM();

	rdtsc = SDL_HasRDTSC();
	mmx = SDL_HasMMX();
	sse = SDL_HasSSE();
	sse2 = SDL_HasSSE2();
	sse3 = SDL_HasSSE3();
	sse41 = SDL_HasSSE41();
	sse42 = SDL_HasSSE42();
	avx = SDL_HasAVX();

	//GPU
	vendor = glGetString(GL_VENDOR);
	gpu_name = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);

	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_memory_kb);

	num_video_drivers = SDL_GetNumVideoDrivers();
	current_video_driver = SDL_GetCurrentVideoDriver();
	for (int i = 0; i < num_video_drivers; i++)
	{
		all_video_drivers.push_back(SDL_GetVideoDriver(i));
	}

	num_render_drivers = SDL_GetNumRenderDrivers();
	render_info = new SDL_RendererInfo();

	//DEVICES
	num_monitors = SDL_GetNumVideoDisplays();
	for (int i = 0; i < num_monitors; i++)
	{
		monitors_name.push_back(SDL_GetDisplayName(i));
	}

	num_joysticks = SDL_NumJoysticks();

	config_menu = true;

	return true;
}

update_status ModuleHardware::Update(float dt)
{
	
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &current_available_memory_kb);
	glGetIntegerv(GPU_MEMORY_INFO_DEDICATED_VIDMEM_NVX, &dedicated_memory_kb);

	memory_usage = total_memory_kb - current_available_memory_kb;

	return UPDATE_CONTINUE;
}

bool ModuleHardware::CleanUp()
{

	if (render_info)
	{
		RELEASE(render_info);
	}

	for (std::vector<string>::iterator item = all_video_drivers.begin(); item != all_video_drivers.end(); item++)
	{
		(*item).clear();
	}
	all_video_drivers.clear();

	for (std::vector<string>::iterator item = monitors_name.begin(); item != monitors_name.end(); item++)
	{
		(*item).clear();
	}
	monitors_name.clear();


	return true;
}

void ModuleHardware::BlitConfigInfo()
{
	//CPU
	ImGui::Text("CPUs:");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i (Cache: %i kB)", num_cpus, cpu_cache);

	//RAM
	ImGui::Text("System RAM: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i MB", system_ram);

	//Caps
	ImGui::Text("Caps: ");
	ImGui::SameLine();

	if (rdtsc)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "RDTSC");
		ImGui::SameLine();
	}

	if (mmx)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "MMX");
		ImGui::SameLine();
	}
		

	if (sse)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "SSE");
		ImGui::SameLine();
	}
		

	if (sse2)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "SSE2");
		ImGui::SameLine();
	}
		

	if (sse3)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "SSE3");
		ImGui::SameLine();
	}
		
	if (sse41)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "SSE41");
		ImGui::SameLine();
	}
		
	if (sse42)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "SSE42");
		ImGui::SameLine();
	}
		

	if (avx)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "AVX");
	}
		
	//GPU
	ImGui::Separator();
	ImGui::Text("GPU: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", vendor);
	
	ImGui::Text("Brand: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", gpu_name);

	ImGui::Text("Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", version);

	ImGui::Text("VRAM budget: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%.2f MB", (total_memory_kb/1000.0f));

	ImGui::Text("VRAM available: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%.2f MB", (current_available_memory_kb / 1000.0f));

	ImGui::Text("VRAM used: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%.2f MB", (memory_usage / 1000.0f));

	ImGui::Text("Number of Video Drivers: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", num_video_drivers);

	ImGui::Text("Current Video Driver: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", current_video_driver.c_str());

	ImGui::Text("Video Drivers: ");
	for (std::vector<string>::iterator item = all_video_drivers.begin(); item != all_video_drivers.end(); item++)
	{
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s, ", (*item).c_str());
	}

	ImGui::Text("Number of Render Drivers: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", num_render_drivers);

	ImGui::Text("Render Drivers: ");
	for (int i = 0; i < num_render_drivers; i++)
	{
		
		ImGui::SameLine();
		SDL_GetRenderDriverInfo(i, render_info);
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s, ", render_info->name);
	}

	//DEVICES
	ImGui::Separator();
	ImGui::Text("DEVICES: ");
	ImGui::Text("Number of Monitors: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", num_monitors);

	ImGui::Text("Monitors: ");
	for (std::vector<string>::iterator item = monitors_name.begin(); item != monitors_name.end(); item++)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "- %s ", (*item).c_str());
	}

	ImGui::Text("Number of Gamepads: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%i", num_joysticks);
}
