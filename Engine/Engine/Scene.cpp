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

	sphere_a.pos = { 0,0,0 };
	sphere_a.r = 0.0f;

	sphere_b.pos = { 0,0,0 };
	sphere_b.r = 0.0f;

	capsule_a.r = 0.0f;
	capsule_a.l.a.Set(0, 0, 0);
	capsule_a.l.b.Set(0, 0, 0);

	capsule_b.r = 0.0f;
	capsule_b.l.a.Set(0, 0, 0);
	capsule_b.l.b.Set(0, 0, 0);

	box_a.SetFromCenterAndSize({ 0,0,0 }, { 0,0,0 });
	box_b.SetFromCenterAndSize({ 0,0,0 }, { 0,0,0 });

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

	//Intersects
	if (ImGui::CollapsingHeader("Intersection Test", false))
	{
		
		if (ImGui::TreeNode("Test Item 1"))
		{
			//Sphere
			if (ImGui::Checkbox("##sphere1", &sphere_1))
			{
				capsule_1 = false;
				aabb_1 = false;
				cylinder_1 = false;
				frustrum_1 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("Sphere##node"))
			{
				ImGui::PushItemWidth(100);
				ImGui::InputFloat("X##2", &sphere_a.pos.x, 0.1, 1.0, 2);
				ImGui::InputFloat("Y##2", &sphere_a.pos.y, 0.1, 1.0, 2);
				ImGui::InputFloat("Z##2", &sphere_a.pos.z, 0.1, 1.0, 2);
				ImGui::InputFloat("Radius", &sphere_a.r,   0.1, 1.0, 2);
				ImGui::PopItemWidth();
				ImGui::TreePop();
			}

			//Capsule
			if (ImGui::Checkbox("##capsule1", &capsule_1))
			{
				sphere_1 = false;
				aabb_1 = false;
				cylinder_1 = false;
				frustrum_1 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("Capsule##node"))
			{

				ImGui::PushItemWidth(100);
				ImGui::InputFloat("A X##4", &capsule_a.l.a.x, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B X##4", &capsule_a.l.b.x, 0.1, 1.0, 2);

				ImGui::InputFloat("A Y##4", &capsule_a.l.a.y, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B Y##4", &capsule_a.l.b.y, 0.1, 1.0, 2);

				ImGui::InputFloat("A Z##4", &capsule_a.l.a.z, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B Z##4", &capsule_a.l.b.z, 0.1, 1.0, 2);

				ImGui::InputFloat("Radius", &capsule_a.r, 0.1, 1.0, 2);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
			
			//AABB
			if (ImGui::Checkbox("##box1", &aabb_1))
			{
				sphere_1 = false;
				capsule_1 = false;
				cylinder_1 = false;
				frustrum_1 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("AABB##node"))
			{
				ImGui::Text("Centre");
				ImGui::SameLine(0,75);
				ImGui::Text("Size in:");

				static math::float3 tmp = { 0,0,0 };
				static math::float3 size = { 0,0,0 };

				ImGui::PushItemWidth(100);
				ImGui::InputFloat("X##6", &tmp.x , 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("X##6", &size.x, 0.1, 1.0, 2);

				ImGui::InputFloat("Y##6", &tmp.y, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("Y##6", &size.y, 0.1, 1.0, 2);

				ImGui::InputFloat("Z##6", &tmp.z, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("Z##6", &size.z, 0.1, 1.0, 2);
				ImGui::PopItemWidth();

				box_a.SetFromCenterAndSize(tmp, size);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Test Item 2"))
		{
			//Sphere 2
			if (ImGui::Checkbox("##sphere2", &sphere_2))
			{
				capsule_2 = false;
				aabb_2 = false;
				cylinder_2 = false;
				frustrum_2 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("Sphere##node2"))
			{
				ImGui::PushItemWidth(100);
				ImGui::InputFloat("X##3", &sphere_b.pos.x, 0.1, 1.0, 2);
				ImGui::InputFloat("Y##3", &sphere_b.pos.y, 0.1, 1.0, 2);
				ImGui::InputFloat("Z##3", &sphere_b.pos.z, 0.1, 1.0, 2);
				ImGui::InputFloat("Radius##1", &sphere_b.r, 0.1, 1.0, 2);
				ImGui::PopItemWidth();
				ImGui::TreePop();
			}

			//Capsule 2
			if (ImGui::Checkbox("##capsule2", &capsule_2))
			{
				sphere_2 = false;
				aabb_2 = false;
				cylinder_2 = false;
				frustrum_2 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("Capsule##node2"))
			{
				ImGui::PushItemWidth(100);
				ImGui::InputFloat("A X##5", &capsule_b.l.a.x, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B Z##5", &capsule_b.l.b.x, 0.1, 1.0, 2);

				ImGui::InputFloat("A Y##5", &capsule_b.l.a.y, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B Z##5", &capsule_b.l.b.y, 0.1, 1.0, 2);

				ImGui::InputFloat("A Z##5", &capsule_b.l.a.z, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("B Z##5", &capsule_b.l.b.z, 0.1, 1.0, 2);

				ImGui::InputFloat("Radius", &capsule_b.r, 0.1, 1.0, 2);
				ImGui::PopItemWidth();

				ImGui::TreePop();
			}
			
			//AABB 2
			//AABB
			if (ImGui::Checkbox("##box1", &aabb_2))
			{
				sphere_2 = false;
				capsule_2 = false;
				cylinder_2 = false;
				frustrum_2 = false;
			}
			ImGui::SameLine();
			if (ImGui::TreeNode("AABB##node2"))
			{
				ImGui::Text("Centre");
				ImGui::SameLine(0, 75);
				ImGui::Text("Size in:");

				static math::float3 tmp_2 = { 0,0,0 };
				static math::float3 size_2 = { 0,0,0 };

				ImGui::PushItemWidth(100);
				ImGui::InputFloat("X##6", &tmp_2.x, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("X##6", &size_2.x, 0.1, 1.0, 2);

				ImGui::InputFloat("Y##6", &tmp_2.y, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("Y##6", &size_2.y, 0.1, 1.0, 2);

				ImGui::InputFloat("Z##6", &tmp_2.z, 0.1, 1.0, 2);
				ImGui::SameLine();
				ImGui::InputFloat("Z##6", &size_2.z, 0.1, 1.0, 2);
				ImGui::PopItemWidth();

				box_b.SetFromCenterAndSize(tmp_2, size_2);

				ImGui::TreePop();
			}

			ImGui::TreePop();
		}



		if (ImGui::Button("Collide?"))
		{
			if (sphere_1)
			{
				if (sphere_2)
				{
					if (sphere_a.Intersects(sphere_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (capsule_2)
				{
					if (sphere_a.Intersects(capsule_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (aabb_2)
				{
					if (sphere_a.Intersects(box_b)) collide_test = "true";
					else collide_test = "false";
				}

			}
			if (capsule_1)
			{
				if (sphere_2)
				{
					if(capsule_a.Intersects(sphere_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (capsule_2)
				{
					if (capsule_a.Intersects(capsule_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (aabb_2)
				{
					if (capsule_a.Intersects(box_b)) collide_test = "true";
					else collide_test = "false";
				}
			}
			if (aabb_1)
			{
				if (sphere_2)
				{
					if (box_a.Intersects(sphere_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (capsule_2)
				{
					if (box_a.Intersects(capsule_b)) collide_test = "true";
					else collide_test = "false";
				}
				if (aabb_2)
				{
					if (box_a.Intersects(box_b)) collide_test = "true";
					else collide_test = "false";
				}
			}

		}
		ImGui::SameLine();
		ImGui::Text("%s", collide_test.c_str());
	}
	
	//ImGui::Text("Line A: Pos(%.2f,%.2f,%.2f) || Dir(%.2f, %.2f, %.2f)", line_a.pos.x, line_a.pos.y, line_a.pos.z, line_a.dir.x, line_a.dir.y, line_a.dir.z);
	
	ImGui::End();

	return update_status::UPDATE_CONTINUE;
}