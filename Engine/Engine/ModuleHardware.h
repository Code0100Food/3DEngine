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
	~ModuleHardware();
	
public:

	bool Start();
	void BlitConfigInfo() {};

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
	const GLubyte* vendor;
	const GLubyte* gpu_name;

	// Get Methods ==================================


};


#endif // !_MODULE_HARDWARE_H_

