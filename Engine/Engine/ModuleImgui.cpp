#include "ModuleImgui.h"
#include "Application.h"
#include "Profiler.h"
#include "ModuleWindow.h"
#include "Scene.h"
#include "ModuleAudio.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui_impl_sdl.h"
#include "Bullet/include/LinearMath/btScalar.h"
#include "ModuleRenderer3D.h"

// Constructors =================================
ModuleImgui::ModuleImgui(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleImgui::~ModuleImgui()
{

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

	//Set imgui style 
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.05f, 0.05f, 0.05f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.9f, 0.9f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.010f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.80f, 0.30f, 1.0f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.64f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	return true;
}

update_status ModuleImgui::Update(float dt)
{
	//return update_status::UPDATE_CONTINUE;

	//Shows if the program has been compiled in debug or release
	ImGui::SetNextWindowPos(ImVec2(0, 30));
	ImGui::SetNextWindowSize(ImVec2(220, 60));
	ImGui::Begin("win", &debug_text, { 250, 100 }, 1.0, ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize);

	//Debug or release mode text
	if (debug_text)
	{
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Compiled in: Debug");
	}
	else
	{
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "Compiled in: Release");
	}
	ImGui::TextColored(ImVec4(0.5f, 0.5f, 1.0f, 1.0f), "%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::End();

	//Top Menu [File, view, tools, help...] -----
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
		{
			App->SetQuit();
		}

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

	//Tools ---------------------------
	cpy = tools_menu_open;
	if (ImGui::BeginMenu("Tools"))
	{
		//Play fx when the menu is opened
		tools_menu_open = true;
		if (tools_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		if (ImGui::MenuItem("Profiler"))
		{
			show_profiler_window = !show_profiler_window;
		}

		if (ImGui::MenuItem("UI Config"))
		{
			show_ui_conf_window = !show_ui_conf_window;
		}
		
		ImGui::EndMenu();
	}
	else if (tools_menu_open)
	{
		tools_menu_open = false;
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
			App->RequestBrowser("https://github.com/Code0100Food/3DEngine/releases/tag/0.1.5");
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
	// ------------------------------------------

	//Profiler window
	if (show_profiler_window)
	{
		App->profiler->BlitInfo(&show_profiler_window);
	}

	//UI Configuration window
	if (show_ui_conf_window)
	{
		BlitUIConfigWindow();
	}

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
	

	return update_status::UPDATE_CONTINUE;
}

bool ModuleImgui::CleanUp()
{
	ImGui::Shutdown();

	return true;
}

void ModuleImgui::BlitUIConfigWindow()
{
	ImGui::Begin("UI Configuration", &show_ui_conf_window);
	ImGui::End();
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

void ModuleImgui::RenderUI()
{
	ImGui::Render();
}

