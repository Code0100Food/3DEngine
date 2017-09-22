#ifndef _MODULE_HARDWARE_H_
#define _MODULE_HARDWARE_H_

#include "Module.h"
#include "SDL/include/SDL_cpuinfo.h"
#include "SDL/include/SDL_opengl.h"



class ModuleHardware : public Module
{
public:
	ModuleHardware();

public:
	~ModuleHardware() {};
	
public:

	bool Start();
	update_status Update(float dt);
	void BlitConfigInfo();

private:

	//CPU
	int num_cpus = 0;
	int cpu_cache = 0;
	int system_ram = 0;

	bool rdtsc = false;
	bool mmx = false;
	bool sse = false;
	bool sse2 = false;
	bool sse3 = false;
	bool sse41 = false;
	bool sse42 = false;
	bool avx = false;

	//GPU
	const GLubyte* vendor = nullptr;
	const GLubyte* gpu_name = nullptr;
	const GLubyte* version = nullptr;

	GLint memory_usage = 0;
	GLint dedicated_memory_kb = 0;
	GLint total_memory_kb = 0;
	GLint current_available_memory_kb = 0;

	// Get Methods ==================================


};


#endif // !_MODULE_HARDWARE_H_

