#include "ModuleHardware.h"

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



	return false;
}
