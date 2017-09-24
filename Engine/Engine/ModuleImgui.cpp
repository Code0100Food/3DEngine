#include "ModuleImgui.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "Scene.h"
#include "ModuleAudio.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui_impl_sdl.h"
#include "Bullet/include/LinearMath/btScalar.h"

// Constructors =================================
ModuleImgui::ModuleImgui(bool start_enabled) : Module(start_enabled)
{
	name = "Imgui";
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

	SDL_GetVersion(&sdl_version);

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

	// File ---------------------------
	bool cpy = file_menu_open;
	if (ImGui::BeginMenu("File"))
	{
		//Play fx when the menu is opened
		file_menu_open = true;
		if (file_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		//Exit button
		if (ImGui::MenuItem("Exit"))
			App->SetQuit();

		ImGui::EndMenu();
	}
	else if (file_menu_open)
	{
		file_menu_open = false;
		App->audio->PlayFxForInput(WINDOW_FX);
	}

	//View ----------------------------
	cpy = view_menu_open;
	if (ImGui::BeginMenu("View"))
	{
		//Play fx when the menu is opened
		view_menu_open = true;
		if (view_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		if (ImGui::MenuItem("Random Generator"))
		{
			App->scene->ShowRandom();
		}

		if (ImGui::MenuItem("Maths Tests"))
		{
			App->scene->ShowMaths();
		}

		if (ImGui::MenuItem("Configuration"))
		{
			App->ShowConfiguration();
		}
		ImGui::EndMenu();
	}
	else if (view_menu_open)
	{
		view_menu_open = false;
		App->audio->PlayFxForInput(WINDOW_FX);
	}

	//Help ----------------------------
	cpy = help_menu_open;
	if (ImGui::BeginMenu("Help"))
	{
		//Play fx when the menu is opened
		help_menu_open = true;
		if (help_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		if (ImGui::MenuItem("Gui Demo"))
		{
			show_test_window = !show_test_window;
		}

		if (ImGui::MenuItem("GitHub Repository"))
		{
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine");
		}

		if (ImGui::MenuItem("Download Last"))
		{
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine/releases");
		}

		if (ImGui::MenuItem("Report a Bug"))
		{
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine/issues");
		}

		if (ImGui::MenuItem("About"))
		{
			show_about_window = !show_about_window;
		}

		ImGui::EndMenu();
	}
	else if (help_menu_open)
	{
		help_menu_open = false;
		App->audio->PlayFxForInput(WINDOW_FX);
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
	ImGui::Render();

	return update_status::UPDATE_CONTINUE;
}

void ModuleImgui::BlitAboutWindow()
{
	ImGui::SetNextWindowSize(ImVec2(500, 500));
	ImGui::Begin("About us", &show_about_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);

	ImGui::Text("Welcome to Fiesta Engine");
	ImGui::Text("This is an engine made by educational purpouse");
	ImGui::Text("We are: Eric Sola and Ferran Martin\n\n");

	ImGui::Text("This engine uses:");
	ImGui::Text("-SDL version: %i.%i.%i", sdl_version.major, sdl_version.minor, sdl_version.patch);
	ImGui::Text("-MathGeoLib version: 1.5");
	ImGui::Text("-ImGui version: %s", ImGui::GetVersion());
	ImGui::Text("-Bullet version: %i", btGetVersion());
	ImGui::Text("-Parson");
	
	ImGui::Text("\nWe don't own all this libraries and are not made by us");
	ImGui::Text("The engine is under MIT License");
	

	ImGui::End();
}

void ModuleImgui::ShowAbout()
{
	show_about_window = !show_about_window;
}

