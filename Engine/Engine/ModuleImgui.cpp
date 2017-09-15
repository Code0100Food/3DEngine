#include "ModuleImgui.h"
#include "Application.h"
#include "ModuleWindow.h"

#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui_impl_sdl.h"

bool ModuleImgui::Start()
{
	show_test_window = true;
	show_another_window = false;
	clear_color = ImColor(144, 244, 0);

	return true;
}

update_status ModuleImgui::Update(float dt)
{
	//Generate the imgui frame
	ImGui_ImplSdl_NewFrame(App->window->window);

	//Exit button
	if (ImGui::Button("Exit",ImVec2(200,50))) App->SetQuit();

	//Test window
	if (show_test_window)
	{
		ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
		ImGui::ShowTestWindow(&show_test_window);
	}

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleImgui::PostUpdate(float dt)
{
	// Rendering
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}
