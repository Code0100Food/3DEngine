#include "ModuleImgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Scene.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui_impl_sdl.h"

// Constructors =================================
ModuleImgui::ModuleImgui(bool start_enabled) : Module(start_enabled)
{
	name = "module_imgui";
}

// Game Loop ====================================
bool ModuleImgui::Start()
{
	show_test_window = false;

#ifdef  _DEBUG
	debug_text = true;
#else 
	debug_text = false;
#endif
	
	clear_color = ImColor(144, 044, 155, 200);
	
	ImGui::GetStyle().Alpha = 1.0;
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);

	return true;
}

update_status ModuleImgui::Update(float dt)
{
	//Shows if the program has been compiled in debug or release
	ImGui::SetNextWindowPos(ImVec2(0, 20));
	ImGui::Begin("win", &debug_text, { 200, 200 }, 0.0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);

	//Debug or release mode text
	if (debug_text)
	{
		ImGui::Text("Compiled in: Debug");
	}
	else
	{
		ImGui::Text("Compiled in: Release");
	}
	ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

	//Top Menu [File, view, help...]
	ImGui::BeginMainMenuBar();
	if (ImGui::BeginMenu("File"))
	{
		//Exit button
		if (ImGui::MenuItem("Exit"))
			App->SetQuit();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("View"))
	{
		if (ImGui::MenuItem("Random Generator"))
			App->scene->ShowRandom();

		if (ImGui::MenuItem("Maths Tests"))
			App->scene->ShowMaths();

		if (ImGui::MenuItem("Configuration"))
			App->ShowConfiguration();

		ImGui::EndMenu();
	}

	if (ImGui::BeginMenu("Help"))
	{
		if(ImGui::MenuItem("Gui Demo"))
			show_test_window = !show_test_window;

		if (ImGui::MenuItem("GitHub Repository"))
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine");

		if (ImGui::MenuItem("Download Last"))
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine/releases");

		if (ImGui::MenuItem("Report a Bug"))
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine/issues");

		if (ImGui::MenuItem("About"))
			show_about_window = !show_about_window;

		ImGui::EndMenu();
	}

	ImGui::EndMainMenuBar();

	//Test window
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 0), ImGuiCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}

	//About Window
	if (show_about_window)
	{
		BlitAboutWindow();
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleImgui::PostUpdate(float dt)
{
	// Rendering
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

void ModuleImgui::BlitAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(100, 100));
	ImGui::Begin("About us", &show_about_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);


	ImGui::End();
}

void ModuleImgui::ShowAbout()
{
	show_about_window = !show_about_window;
}

