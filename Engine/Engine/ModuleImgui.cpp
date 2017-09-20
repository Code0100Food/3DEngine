#include "ModuleImgui.h"
#include "Application.h"
#include "ModuleWindow.h"
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
	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);

	//Shows if the program has been compiled in debug or release
	ImGui::Begin("win", &debug_text, { 200, 200 }, 0.0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);
	ImGui::SetWindowPos(ImVec2(0, 0));

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


	//Exit button
	if (ImGui::Button("Exit", ImVec2(200, 50))) App->SetQuit();
	//Show hide button
	if (ImGui::Button("Show/Hide Demo", ImVec2(200, 50)))show_test_window = !show_test_window;

	ImGui::End();

	//Test window
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 0), ImGuiCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
		
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
