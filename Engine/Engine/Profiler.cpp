#include "Profiler.h"

// Constructors =================================
Profiler::Profiler(bool start_enabled) :Module(start_enabled)
{
	id = M_PROFILER;
	name = "Profiler";
	config_menu = true;
}

// Destructors ==================================
Profiler::~Profiler()
{

}

// Game Loop ====================================
void Profiler::BlitConfigInfo()
{
	ImGui::Text("App Build: %.3f (Sec)", GetProfBlock(APPLICATION,BUILD_STEP)->time_in_nanoseconds / 1000000.0f);
}

// Functionality ================================
void Profiler::CallProfBlock(MODULE_ID id, LOOP_STEP step, uint64 time)
{
	bool found = false;

	//Search a prof block with the same id
	uint size = current_profiled_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//If we found id we can simply update the time value
		if (current_profiled_blocks[k].module_id == id && current_profiled_blocks[k].loop_step == step)
		{
			current_profiled_blocks[k].time_in_nanoseconds = time;
			found = true;
		}
	}

	//Else Build and add a prof block to the current blocks vector
	if(!found)current_profiled_blocks.push_back({ id,step,time });
}

Prof_Block * Profiler::GetProfBlock(MODULE_ID id, LOOP_STEP step)const
{
	Prof_Block* ptr = nullptr;

	//Search a prof block with the same id
	uint size = current_profiled_blocks.size();
	for (uint k = 0; k < size; k++)
	{
		//If we found id we point the ptr to it
		if (current_profiled_blocks[k].module_id == id && current_profiled_blocks[k].loop_step == step)
		{
			ptr = (Prof_Block*)&current_profiled_blocks[k];
			break;
		}
	}

	return ptr;
}
