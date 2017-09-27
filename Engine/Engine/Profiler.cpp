#include "Profiler.h"

// Constructors =================================
Profiler::Profiler()
{
	
}

// Destructors ==================================
Profiler::~Profiler()
{

}

// Game Loop ====================================
void Profiler::BlitInfo()
{
	//Build configuration base window
	ImGui::SetNextWindowPos(ImVec2(0, 80));
	ImGui::SetNextWindowSize(ImVec2(300, 360));
	ImGui::Begin("Profiler", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
	ImGui::TextColored(ImVec4(0.4, 0.8, 0.8, 1.0), "Profiler");

	Prof_Block* block_ptr = nullptr;

	//Show app time tracking --------------------
	ImGui::Text("App:");
	BlitModuleProfile(APPLICATION, true);
	ImGui::Separator();

	//Show console time tracking ----------------
	ImGui::Text("Console:");
	BlitModuleProfile(M_CONSOLE);
	ImGui::Separator();

	//Show audio time tracking ------------------
	ImGui::Text("Audio:");
	BlitModuleProfile(M_AUDIO);
	ImGui::Separator();

	//Show camera time tracking -----------------
	ImGui::Text("Camera:");
	BlitModuleProfile(M_CAMERA3D);
	ImGui::Separator();

	//Show imgui time tracking ------------------
	ImGui::Text("ImGui:");
	BlitModuleProfile(M_IMGUI);
	ImGui::Separator();

	//Show input time tracking ------------------
	ImGui::Text("Input:");
	BlitModuleProfile(M_INPUT);
	ImGui::Separator();

	//Show physics time tracking ----------------
	ImGui::Text("Physics:");
	BlitModuleProfile(M_PHYSICS3D);
	ImGui::Separator();

	//Show window time tracking -----------------
	ImGui::Text("Window:");
	BlitModuleProfile(M_WINDOW);
	ImGui::Separator();

	//Show scene time tracking ------------------
	ImGui::Text("Scene:");
	BlitModuleProfile(M_SCENE);
		
	ImGui::End();
}

// Functionality ================================
void Profiler::BlitModuleProfile(MODULE_ID id, bool graph)
{
	Prof_Block* block_ptr = nullptr;

	//Build -----------------
	block_ptr = GetProfBlock(id, BUILD_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Build		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Awake -----------------
	block_ptr = GetProfBlock(id, AWAKE_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Awake		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Init ------------------
	block_ptr = GetProfBlock(id, INIT_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Init		 "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Start -----------------
	block_ptr = GetProfBlock(id, START_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Start		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Sec)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//PreUpdate -------------
	block_ptr = GetProfBlock(id, PRE_UPDATE_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("PreUpdate	"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	}
	//Update ----------------
	block_ptr = GetProfBlock(id, UPDATE_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Update	   "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	}
	//PostUpdate ------------
	block_ptr = GetProfBlock(id, POST_UPDATE_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("PostUpdate   "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000.0f);
	}

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
		ImGui::PlotHistogram("", time_values, 3, 30, title, 0.0f,30.0f, ImVec2(180, 50));

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
