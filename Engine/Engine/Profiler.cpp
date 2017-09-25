#include "Profiler.h"

// Constructors =================================
Profiler::Profiler(bool start_enabled) :Module(start_enabled)
{
	name = "Profiler";
}

// Destructors ==================================
Profiler::~Profiler()
{

}

// Game Loop ====================================
bool Profiler::Awake(const JSON_Object * data_root)
{
	config_menu = true;

	return true;
}
