#include "ModuleHardware.h"
#include <gl/GL.h>
#include <gl/GLU.h>


// Constructors =================================
ModuleHardware::ModuleHardware(bool start_enabled) :Module(start_enabled)
{
	name = "Hardware";
}

// Destructors ==================================
ModuleHardware::~ModuleHardware()
{

}


// Game Loop ====================================
bool ModuleHardware::Start()
{
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

	vendor = glGetString(GL_VENDOR);
	gpu_name = glGetString(GL_RENDERER);
	version = glGetString(GL_VERSION);

	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_memory_kb);

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


}
