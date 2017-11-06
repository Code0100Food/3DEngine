#include "Profiler.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleImgui.h"

// Constructors =================================
Profiler::Profiler()
{
	
}

// Destructors ==================================
Profiler::~Profiler()
{

}

// Game Loop ====================================
void Profiler::BlitInfo(bool* flag)
{
	//Build configuration base window
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.4f, (App->window->GetHeight() - 23) * 0.5f), App->imgui->GetUICondFlag());
	ImGui::SetNextWindowPos(ImVec2(0, (App->window->GetHeight() - 23) * 0.5f + 23), App->imgui->GetUICondFlag());
	ImGui::Begin("Profiler", flag, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
	ImGui::TextColored(ImVec4(0.4, 0.8, 0.8, 1.0), "Profiler");
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Shows how long each module's step takes");

	Prof_Block* block_ptr = nullptr;

	//Show app time tracking --------------------
	ImGui::Text("App:");
	BlitModuleProfile(APPLICATION, "##App", true);
	ImGui::Separator();

	//Show console time tracking ----------------
	ImGui::Text("Console:");
	BlitModuleProfile(M_CONSOLE,"##Console");
	ImGui::Separator();

	//Show audio time tracking ------------------
	ImGui::Text("Audio:");
	BlitModuleProfile(M_AUDIO,"##Audio");
	ImGui::Separator();

	//Show camera time tracking -----------------
	ImGui::Text("Camera:");
	BlitModuleProfile(M_CAMERA3D,"##Camera");
	ImGui::Separator();

	//Show imgui time tracking ------------------
	ImGui::Text("ImGui:");
	BlitModuleProfile(M_IMGUI,"##ImGui");
	ImGui::Separator();

	//Show input time tracking ------------------
	ImGui::Text("Input:");
	BlitModuleProfile(M_INPUT,"##Input");
	ImGui::Separator();

	//Show window time tracking -----------------
	ImGui::Text("Window:");
	BlitModuleProfile(M_WINDOW,"##Window");
	ImGui::Separator();

	//Show renderer time tracking ---------------
	ImGui::Text("Renderer:");
	BlitModuleProfile(M_RENDERER, "##Renderer");
	ImGui::Separator();

	//Show scene time tracking ------------------
	ImGui::Text("Scene:");
	BlitModuleProfile(M_SCENE,"##Scene");
		
	ImGui::End();
}

bool Profiler::LoadConfiguration(const JSON_Object * data_root)
{
	//Load all the modules max milliseconds
	SetMilliLimit(APPLICATION, json_object_get_number(data_root, "Application"));
	SetMilliLimit(M_CONSOLE, json_object_get_number(data_root, "Console"));
	SetMilliLimit(M_FILE_SYSTEM, json_object_get_number(data_root, "FileSystem"));
	SetMilliLimit(M_AUDIO, json_object_get_number(data_root, "Audio"));
	SetMilliLimit(M_CAMERA3D, json_object_get_number(data_root, "Camera"));
	SetMilliLimit(M_HARDWARE, json_object_get_number(data_root, "Hardware"));
	SetMilliLimit(M_IMGUI, json_object_get_number(data_root, "ImGui"));
	SetMilliLimit(M_INPUT, json_object_get_number(data_root, "Input"));
	SetMilliLimit(M_INPUT_MANAGER, json_object_get_number(data_root, "InputManager"));
	SetMilliLimit(M_RENDERER, json_object_get_number(data_root, "Renderer"));
	SetMilliLimit(M_WINDOW, json_object_get_number(data_root, "Window"));
	SetMilliLimit(M_SCENE, json_object_get_number(data_root, "Scene"));

	return true;
}

void Profiler::SaveConfiguration(JSON_Object * data_root) const
{
	//Save all the modules max milliseconds
	json_object_set_number(data_root, "Application", GetMilliLimit(APPLICATION)->second);
	json_object_set_number(data_root, "Console", GetMilliLimit(M_CONSOLE)->second);
	json_object_set_number(data_root, "FileSystem", GetMilliLimit(M_FILE_SYSTEM)->second);
	json_object_set_number(data_root, "Audio", GetMilliLimit(M_AUDIO)->second);
	json_object_set_number(data_root, "Camera", GetMilliLimit(M_CAMERA3D)->second);
	json_object_set_number(data_root, "Hardware", GetMilliLimit(M_HARDWARE)->second);
	json_object_set_number(data_root, "ImGui", GetMilliLimit(M_IMGUI)->second);
	json_object_set_number(data_root, "Input", GetMilliLimit(M_INPUT)->second);
	json_object_set_number(data_root, "InputManager", GetMilliLimit(M_INPUT_MANAGER)->second);
	json_object_set_number(data_root, "Renderer", GetMilliLimit(M_RENDERER)->second);
	json_object_set_number(data_root, "Window", GetMilliLimit(M_WINDOW)->second);
	json_object_set_number(data_root, "Scene", GetMilliLimit(M_SCENE)->second);
}

// Functionality ================================
void Profiler::BlitModuleProfile(MODULE_ID id, const char* str_id, bool graph)
{
	Prof_Block* block_ptr = nullptr;
	std::pair<MODULE_ID, float>* milli_limit = GetMilliLimit(id);

	//Build -----------------
	block_ptr = GetProfBlock(id, BUILD_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Build		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Awake -----------------
	block_ptr = GetProfBlock(id, AWAKE_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Awake		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Init ------------------
	block_ptr = GetProfBlock(id, INIT_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Init		 "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}
	//Start -----------------
	block_ptr = GetProfBlock(id, START_STEP);
	if (block_ptr != nullptr)
	{
		ImGui::BulletText("Start		"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", block_ptr->time_in_nanoseconds / 1000000.0f);
	}

	//Game Loop -------------
	Prof_Block* pre_block_ptr = GetProfBlock(id, PRE_UPDATE_STEP);
	float pre_update_ms = pre_block_ptr != nullptr ? pre_block_ptr->time_in_nanoseconds / 1000.0f : 0.000f;
	Prof_Block* up_block_ptr = GetProfBlock(id, UPDATE_STEP);
	float update_ms = up_block_ptr != nullptr ? up_block_ptr->time_in_nanoseconds / 1000.0f : 0.000f;
	Prof_Block* post_block_ptr = GetProfBlock(id, POST_UPDATE_STEP);
	float post_update_ms = post_block_ptr != nullptr ? post_block_ptr->time_in_nanoseconds / 1000.0f : 0.000f;

	//PreUpdate -------------
	if (pre_block_ptr != nullptr)
	{
		ImGui::BulletText("PreUpdate	"); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", pre_update_ms);
	}
	//Update ----------------
	block_ptr = GetProfBlock(id, UPDATE_STEP);
	if (up_block_ptr != nullptr)
	{
		ImGui::BulletText("Update	   "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", update_ms);
	}
	//PostUpdate ------------
	block_ptr = GetProfBlock(id, POST_UPDATE_STEP);
	if (post_block_ptr != nullptr)
	{
		ImGui::BulletText("PostUpdate   "); ImGui::SameLine();
		ImGui::TextColored(ImVec4(0.5, 1.0, 0.5, 1.0), "%.3f (Mil)", post_update_ms);
	}

	//Max Milliseconds ------
	if (milli_limit != nullptr)
	{
		float limit = milli_limit->second;
		float total = pre_update_ms + update_ms + post_update_ms;
		ImGui::Text("State: "); 
		ImGui::SameLine();
		if (total > limit)ImGui::TextColored(ImVec4(0.8, 0.2, 0.2, 1.0), "EXCEED");
		else if (total >= (float)limit * 0.5f)ImGui::TextColored(ImVec4(0.2, 0.8, 0.2, 1.0), "NORMAL");
		else if(total < (float)limit * 0.5f)ImGui::TextColored(ImVec4(0.5, 0.2, 0.7, 1.0), "PERFECT");

		ImGui::Text("Max Ms");
		ImGui::SameLine();
		ImGui::SliderFloat(str_id, &milli_limit->second, 0, MAX_MS,"%.1f");
	}

	//Represents all the data in a graph
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
		char title[50];
		sprintf_s(title, 50, "Total Milliseconds %.2f", (time_values[0] + time_values[1] + time_values[2]));
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
			break;
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

void Profiler::SetMilliLimit(MODULE_ID id, float limit)
{
	bool in = false;
	//Check if the send pair is already in the vector
	std::vector<std::pair<MODULE_ID, float>>::const_iterator pair = modules_max_milli.begin();
	while (pair != modules_max_milli.end())
	{
		if (pair._Ptr->first == id)
		{
			pair._Ptr->second = limit;
			in = true;
			break;
		}
		pair++;
	}
	//Else add the new pair in the vector
	if(!in)modules_max_milli.push_back({ id,limit });
}

std::pair<MODULE_ID, float>* Profiler::GetMilliLimit(MODULE_ID id) const
{
	//Check if the send pair is already in the vector
	std::vector<std::pair<MODULE_ID, float>>::const_iterator pair = modules_max_milli.begin();
	while (pair != modules_max_milli.end())
	{
		if (pair._Ptr->first == id)return pair._Ptr;

		pair++;
	}
	
	return nullptr;
}
