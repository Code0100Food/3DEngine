#include "Scene.h"
#include "Application.h"
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
	ImGui::SetNextWindowSize(ImVec2(300, 170));
	ImGui::Begin("Random Test", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar);
	
	//Random number button
	if (ImGui::Button("Randomize", ImVec2(80, 35)))
	{

		random_float = rand_num_generator->Float();
	}

	//Show the resulting number in a string
	ImGui::SameLine(10, 100);
	ImGui::Text("%f",random_float);

	//Random number_2 values
	ImGui::InputInt("A", &val_a, 1, 10);
	ImGui::InputInt("B", &val_b, 1, 10);
	
	if(ImGui::Button("Randomize##1",ImVec2(100,35)))
	{
		val_a < val_b ? random_int = rand_num_generator->Int(val_a, val_b) : random_int = rand_num_generator->Int(val_b, val_a);
	}
	
	ImGui::Text("Randomized: %i", random_int);

	ImGui::End();

	//Primitives output window
	ImGui::SetNextWindowPos(ImVec2(1070, 100));
	ImGui::SetNextWindowSize(ImVec2(200, 100));
	ImGui::Begin("Primitives", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
	
	//Primitives Test
	
	math::Sphere* test = App->physics->CreateSphere({ 0,0,0 }, 5);
	math::Sphere* test_2 = App->physics->CreateSphere({ 2,2,2 }, 5);


	ImGui::LabelText("SPHERE", "Value");
	ImGui::LabelText("SPHERE", "Value");

	if (test->Intersects(*test_2))
	{
		ImGui::Text("Intersects");
	}

	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}