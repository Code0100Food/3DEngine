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
	//Show app time tracking
	ImGui::Text("App:");
	ImGui::Text("- Build		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", GetProfBlock(APPLICATION, BUILD_STEP)->time_in_nanoseconds / 1000000.0f);
	//Show app awake time
	ImGui::Text("- Awake		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", GetProfBlock(APPLICATION, AWAKE_STEP)->time_in_nanoseconds / 1000000.0f);
	//Show app start time
	ImGui::Text("- Start		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", GetProfBlock(APPLICATION, START_STEP)->time_in_nanoseconds / 1000000.0f);
	//Show app pre update time
	ImGui::Text("- PreUpdate	"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(APPLICATION, PRE_UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	//Show app update time
	ImGui::Text("- Update	   "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(APPLICATION, UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	//Show app post update time
	ImGui::Text("- PostUpdate   "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(APPLICATION, POST_UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	ImGui::Separator();
	//Show renderer time tracking
	ImGui::Text("Renderer:");
	ImGui::Text("- Build		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(M_RENDERER, BUILD_STEP)->time_in_nanoseconds / 1000.0f);
	//Show app awake time
	/*ImGui::Text("- Awake		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", GetProfBlock(M_RENDERER, AWAKE_STEP)->time_in_nanoseconds / 1000000.0f);
	//Show app start time
	ImGui::Text("- Start		"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", GetProfBlock(M_RENDERER, START_STEP)->time_in_nanoseconds / 1000000.0f);
	//Show app pre update time
	ImGui::Text("- PreUpdate	"); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(M_RENDERER, PRE_UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	//Show app update time
	ImGui::Text("- Update	   "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(M_RENDERER, UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	//Show app post update time
	ImGui::Text("- PostUpdate   "); ImGui::SameLine();
	ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", GetProfBlock(M_RENDERER, POST_UPDATE_STEP)->time_in_nanoseconds / 1000.0f);
	*/ImGui::Separator();
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
