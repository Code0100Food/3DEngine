#ifndef _MODULE_HARDWARE_H_
#define _MODULE_HARDWARE_H_

#include "Module.h"
#include "SDL/include/SDL_cpuinfo.h"
#include "SDL/include/SDL_opengl.h"
#include "SDL/include/SDL.h"


class ModuleHardware : public Module
{
public:

	ModuleHardware(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleHardware();
	
public:

	bool			Start()final;
	update_status	Update(float dt)final;
	bool			CleanUp() final;
	void			BlitConfigInfo()final;

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

	int					num_video_drivers = 0;
	string				current_video_driver;
	vector<string>		all_video_drivers;

	int					num_render_drivers = 0;
	SDL_RendererInfo*	render_info = nullptr;

	//DEVICES
	int				num_monitors = 0;
	vector<string>	monitors_name;

	int				num_joysticks = 0;

};


#endif // !_MODULE_HARDWARE_H_

