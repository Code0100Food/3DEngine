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
	Prof_Block* block_ptr = nullptr;

	//Show app time tracking --------------------
	ImGui::Text("App:");
	BlitModuleProfile(APPLICATION, true);
	ImGui::Separator();

	//Show renderer time tracking ---------------
	ImGui::Text("Renderer:");
	BlitModuleProfile(M_RENDERER);
	ImGui::Separator();

	//Show console time tracking ---------------
	ImGui::Text("Console:");
	BlitModuleProfile(M_CONSOLE);
	ImGui::Separator();

	//Show camera time tracking ---------------
	ImGui::Text("Camera:");
	BlitModuleProfile(M_CAMERA3D);
	ImGui::Separator();
		
}

// Functionality ================================
void Profiler::BlitModuleProfile(MODULE_ID id, bool graph)
{
	Prof_Block* block_ptr = nullptr;

	ImGui::Text("- Build		"); ImGui::SameLine();
	block_ptr = GetProfBlock(id, BUILD_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//Awake -----------------
	ImGui::Text("- Awake		"); ImGui::SameLine();
	block_ptr = GetProfBlock(id, AWAKE_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//Init ------------------
	ImGui::Text("- Init			"); ImGui::SameLine();
	block_ptr = GetProfBlock(id, INIT_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//Start -----------------
	ImGui::Text("- Start		"); ImGui::SameLine();
	block_ptr = GetProfBlock(id, START_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//PreUpdate -------------
	ImGui::Text("- PreUpdate	"); ImGui::SameLine();
	block_ptr = GetProfBlock(id, PRE_UPDATE_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//Update ----------------
	ImGui::Text("- Update	   "); ImGui::SameLine();
	block_ptr = GetProfBlock(id, UPDATE_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");
	//PpstUpdate ------------
	ImGui::Text("- PostUpdate   "); ImGui::SameLine();
	block_ptr = GetProfBlock(id, POST_UPDATE_STEP);
	if (block_ptr != nullptr)ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	else ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "- disabled -");

	//Reperesents all the data in agraph
	if (graph)
	{
		//Blit milliseconds graphic
		float time_values[3];
		block_ptr = GetProfBlock(id, PRE_UPDATE_STEP);
		time_values[0] = block_ptr == nullptr ? 0 : block_ptr->time_in_nanoseconds / 1000.0f;
		block_ptr = GetProfBlock(id, UPDATE_STEP);
		time_values[1] = block_ptr == nullptr ? 0 : block_ptr->time_in_nanoseconds / 1000.0f;
		block_ptr = GetProfBlock(id, POST_UPDATE_STEP);
		time_values[2] = block_ptr == nullptr ? 0 : block_ptr->time_in_nanoseconds / 1000.0f;
		char title[25];
		sprintf_s(title, 25, "Total Miliseconds %.2f", (time_values[0] + time_values[1] + time_values[2]));
		ImGui::PlotHistogram("", time_values, 3, 30, title, 0.0f, 100.0f, ImVec2(180, 50));

	}
}

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
