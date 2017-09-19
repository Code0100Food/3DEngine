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

	//Initialize all the primitives
	line_a.pos = { 0,0,0 };
	line_a.dir = { 0,0,0 };

	line_b.pos = { 0,0,0 };
	line_b.dir = { 0,0,0 };

	cross_product.pos = { 0,0,0 };
	cross_product.dir = { 0,0,0 };

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
	ImGui::SetNextWindowPos(ImVec2(850, 100));
	ImGui::SetNextWindowSize(ImVec2(400, 400));
	ImGui::Begin("Primitives", &open, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
	

	//Primitives Test
	//if (ImGui::BeginMenu("Vectors")) { ImGui::EndMenu(); };
	
	//Vectors
	if (ImGui::CollapsingHeader("Vectors", false))
	{
		ImGui::Text("Vector 1");
		ImGui::SameLine(120);
		ImGui::Text("Vector 2");

		ImGui::PushItemWidth(100);
		ImGui::InputFloat("X", &line_a.dir.x, 0.1, 1.0, 2);
		ImGui::SameLine();
		ImGui::InputFloat("X##1", &line_b.dir.x, 0.1, 1.0, 2);

		ImGui::InputFloat("Y", &line_a.dir.y, 0.1, 1.0, 2);
		ImGui::SameLine();
		ImGui::InputFloat("Y##1", &line_b.dir.y, 0.1, 1.0, 2);

		ImGui::InputFloat("Z", &line_a.dir.z, 0.1, 1.0, 2);
		ImGui::SameLine();
		ImGui::InputFloat("Z##1", &line_b.dir.z, 0.1, 1.0, 2);
		ImGui::PopItemWidth();

		if (ImGui::Button("Dot Product", ImVec2(100, 20)))
		{
			dot_product = line_a.dir.Dot(line_b.dir);
		}

		ImGui::SameLine();
		ImGui::Text("%.2f", dot_product);

		if (ImGui::Button("Cross Product", ImVec2(100, 20)))
		{
			cross_product.dir = line_a.dir.Cross(line_b.dir);
		}

		ImGui::SameLine();
		ImGui::Text(" X: %.2f Y: %.2f Z: %.2f", cross_product.dir.x, cross_product.dir.y, cross_product.dir.z);


	}

	//Spheres
	if (ImGui::CollapsingHeader("Spheres", false))
	{

	}
	
	//ImGui::Text("Line A: Pos(%.2f,%.2f,%.2f) || Dir(%.2f, %.2f, %.2f)", line_a.pos.x, line_a.pos.y, line_a.pos.z, line_a.dir.x, line_a.dir.y, line_a.dir.z);
	
	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}