#include "ModuleHardware.h"
#include <gl/GL.h>
#include <gl/GLU.h>

ModuleHardware::ModuleHardware()
{
	name = "module_hardware";
}

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

	return true;
}

update_status ModuleHardware::Update(float dt)
{
	glGetIntegerv(GPU_MEMORY_INFO_TOTAL_AVAILABLE_MEMORY_NVX, &total_memory_kb);
	glGetIntegerv(GPU_MEMORY_INFO_CURRENT_AVAILABLE_VIDMEM_NVX, &current_available_memory_kb);
	memory_usage = total_memory_kb - current_available_memory_kb;

	return UPDATE_CONTINUE;
}
