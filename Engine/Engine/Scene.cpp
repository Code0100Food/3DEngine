#include "Scene.h"
#include "ModulePhysics3D.h"
#include "imgui/imgui.h"

// Constructors =================================
Scene::Scene(Application * parent, bool start_enabled) : Module(parent, start_enabled)
{

}

// Destructors ==================================
Scene::~Scene()
{

}

// Game Loop ====================================
bool Scene::Start()
{
	//Starts the random number generator
	rand_num_generator = new math::LCG((u32)5);

	return true;
}

update_status Scene::Update(float dt)
{
	//Prepare random number window
	bool open = true;
	ImGui::SetNextWindowPos(ImVec2(0, 150));
	ImGui::SetNextWindowSize(ImVec2(200, 80));
	ImGui::Begin("Random Test", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
	
	//Random number button
	if (ImGui::Button("Randomize", ImVec2(80, 35)))
	{
		float rand_number = rand_num_generator->Float();
		char buffer[10];
		itoa(rand_number, buffer, 10);
		strcat(buffer, ".");
		uint16_t decimal_part = (rand_number - (int)rand_number) * 10000;
		char buffer_2[10];
		itoa(decimal_part, buffer_2, 10);
		strcat(buffer, buffer_2);
		rand_num_string = buffer;
	}

	//Show the resulting number in a string
	ImGui::SameLine(10, 100);
	ImGui::Text(rand_num_string.c_str());

	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}