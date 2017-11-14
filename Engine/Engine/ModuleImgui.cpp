#include "ModuleImgui.h"
#include "Application.h"
#include "Profiler.h"
#include "ModuleWindow.h"
#include "ModuleAudio.h"
#include "SDL/include/SDL_opengl.h"
#include "imgui/imgui_impl_sdl.h"
#include "Bullet/include/LinearMath/btScalar.h"
#include "ModuleRenderer3D.h"
#include "GeometryManager.h"
#include "imgui/imgui_dock.h"
#include "Devil/include/il.h"
#include "ModuleRenderer3D.h"
#include "ModuleScene.h"
#include "FileSystem.h"

// Constructors =================================
ModuleImgui::ModuleImgui(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleImgui::~ModuleImgui()
{

}

// Game Loop ====================================
bool ModuleImgui::Awake(const JSON_Object * data_root)
{
	//Load theme booleans
	dark_theme = json_object_get_boolean(data_root, "dark_theme");
	light_theme = json_object_get_boolean(data_root, "light_theme");
	custom_theme = json_object_get_boolean(data_root, "custom_theme");
	
	//Load all custom theme data
	const JSON_Object * custom_theme_root = json_object_get_object(data_root, "custom_theme_data");
	const JSON_Object * custom_theme_color_root = json_object_get_object(custom_theme_root, "colors");
	for (uint k = 0; k < ImGuiCol_COUNT; k++)
	{
		char index[5];
		itoa(k, index, 10);
		string number = index;
		string next_win = "color" + number;

		JSON_Array* color_array = json_object_get_array(custom_theme_color_root, next_win.c_str());
		custom_style.Colors[k].x = json_array_get_number(color_array, 0);
		custom_style.Colors[k].y = json_array_get_number(color_array, 1);
		custom_style.Colors[k].z = json_array_get_number(color_array, 2);
		custom_style.Colors[k].w = json_array_get_number(color_array, 3);
		
		number.clear();
		next_win.clear();
	}

	return true;
}
bool ModuleImgui::Start()
{
	show_test_window = false;
	
	cond_flag_time = 500;

	ImGui::GetStyle().Alpha = 1.0;
	ImGui::GetStyle().WindowTitleAlign = ImVec2(0.5, 0.5);

	SDL_GetVersion(&sdl_version);

	//Build custom style basics
	custom_style.WindowPadding = ImVec2(15, 15);
	custom_style.WindowRounding = 5.0f;
	custom_style.FramePadding = ImVec2(5, 5);
	custom_style.FrameRounding = 4.0f;
	custom_style.ItemSpacing = ImVec2(12, 8);
	custom_style.ItemInnerSpacing = ImVec2(8, 6);
	custom_style.IndentSpacing = 25.0f;
	custom_style.ScrollbarSize = 15.0f;
	custom_style.ScrollbarRounding = 9.0f;
	custom_style.GrabMinSize = 5.0f;
	custom_style.GrabRounding = 3.0f;

	//Set imgui style 
	if (custom_theme)SetCustomTheme();
	else if (dark_theme)SetDarkTheme();
	else if (light_theme)SetLightTheme();

	//father dock
	father_dock = new DockContext();

	char str[50];
	sprintf(str, "%sdock_settings.json", SETTINGS_FOLDER);
	JSON_Value* dock_settings = json_parse_file(str);

	father_dock->LoadDock(dock_settings);

	json_value_free(dock_settings);

	return true;
}

update_status ModuleImgui::PreUpdate(float dt)
{
	

	return update_status::UPDATE_CONTINUE;
}

update_status ModuleImgui::Update(float dt)
{
	App->renderer3D->DisableGLRenderFlags();

	//Exit Window
	if (show_exit_window)
	{
		BlitExitWindow();
	}

	//Top Menu [File, view, tools, help...] -----
	ImGui::BeginMainMenuBar();

	// File ---------------------------
	bool cpy = file_menu_open;
	if (ImGui::BeginMenu("File"))
	{
		//Play fx when the menu is opened
		file_menu_open = true;
		if (file_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		//Save button
		if (ImGui::MenuItem("Save"))
		{
			App->scene->SerializeAndSaveCurrentScene();
		}

		//Load button
		if (ImGui::MenuItem("Load"))
		{
			App->scene->CleanScene();
			char str[50];
			sprintf(str, "%sscene.json", LIBRARY_FOLDER);
			App->scene->LoadSerializedScene(str);
		}

		//Clean Button
		if (ImGui::MenuItem("Clean"))
		{
			App->scene->CleanScene();
		}
		
		//Exit button
		if (ImGui::MenuItem("Exit"))
		{
			show_exit_window = true;
		}

		ImGui::EndMenu();
	}
	else if (file_menu_open)
	{
		file_menu_open = false;
		App->audio->PlayFxForInput(WINDOW_FX);
	}

	//Game Objects --------------------
	cpy = gameobjects_open;
	if (ImGui::BeginMenu("Game Objects"))
	{
		//Play fx when the menu is opened
		gameobjects_open = true;
		if (gameobjects_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		//Create a empty game object
		if (ImGui::MenuItem("Empty"))
		{
			GameObject* empty_obj = App->scene->CreateGameObject();
			empty_obj->CreateComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
			App->scene->SetSelectedGameObject(empty_obj);
		}

		//Create Cube
		if (ImGui::MenuItem("Cube"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_CUBE));
		}

		//Create Sphere
		if (ImGui::MenuItem("Sphere (LowPoly)"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_SPHERE));
		}

		//Create Sphere
		if (ImGui::MenuItem("Sphere (HiPoly)"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_SPHERE_HI));
		}

		//Create Cylinder
		if (ImGui::MenuItem("Cylinder (LowPoly)"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER));
		}

		//Create Cylinder
		if (ImGui::MenuItem("Cylinder (HiPoly)"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_CYLINDER_HI));
		}

		//Create Frustum
		if (ImGui::MenuItem("Frustum (InProcess)"))
		{
			App->scene->SetSelectedGameObject(App->scene->CreatePrimitive(PRIMITIVE_TYPE::PRIMITIVE_FRUSTUM));
		}


		ImGui::EndMenu();
	}
	else if (gameobjects_open)
	{
		gameobjects_open = false;
		App->audio->PlayFxForInput(WINDOW_FX);
	}

	//View ----------------------------
	cpy = view_menu_open;
	if (ImGui::BeginMenu("View"))
	{
		//Play fx when the menu is opened
		view_menu_open = true;
		if (view_menu_open != cpy)App->audio->PlayFxForInput(WINDOW_FX);

		if (ImGui::MenuItem("Hierarchy"))
		{
			App->scene->SwapHierarchyWinState();
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

	//Play / Pause / Next buttons
	ImGui::SetNextWindowPos({ 0, 15 }, cond_flag);
	ImGui::SetNextWindowSize({ (float)App->window->GetWidth(), 45}, cond_flag);
	ImGui::Begin("PlayPauseNext", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoMove | ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);
	App->renderer3D->PrintPlayPauseButton();
	ImGui::End();

	//Docks
	ImGui::SetNextWindowPos({ 0, 45 }, cond_flag);
	ImGui::SetNextWindowSize({ (float)App->window->GetWidth(), (float)App->window->GetHeight() - 45 }, cond_flag);
	ImGui::Begin("", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

	father_dock->BeginWorkspace("##father_workspace");

	//-------------------------------------------

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

	//Config Window
	if (App->GetConfigWindowState())
	{
		App->BlitConfigWindow();
	}

	//Hierarchy Window
	if (App->scene->GetHierarchyWinState())
	{
		father_dock->BeginDock("Hierarchy##Dock", 0, 0);
		App->scene->BlitHierarchy();
		father_dock->EndDock();
	}

	//Inspector Window
	if (App->scene->GetInspectorState())
	{
		father_dock->BeginDock("Inspector##Dock", 0, 0);
		App->scene->BlitInspector();
		father_dock->EndDock();
	}

	//Directories Window
	App->fs->BlitFileSystemInfo();

	App->renderer3D->EnableGLRenderFlags();

	//Update cond_flag 
	if (cond_flag != ImGuiCond_::ImGuiCond_Once && cond_flag_timer.Read() > cond_flag_time)
	{
		cond_flag = ImGuiCond_::ImGuiCond_Once;
	}

	return update_status::UPDATE_CONTINUE;
}

bool ModuleImgui::CleanUp()
{
	//father_dock->SaveDocks();
	RELEASE(father_dock);
	ImGui::Shutdown();

	return true;
}

void ModuleImgui::SaveConfigInfo(JSON_Object * data_root)
{
	//Save theme booleans
	json_object_set_boolean(data_root, "dark_theme", dark_theme);
	json_object_set_boolean(data_root, "light_theme", light_theme);
	json_object_set_boolean(data_root, "custom_theme", custom_theme);

	//Save all the custom theme data
	JSON_Object * custom_theme_root = json_object_get_object(data_root, "custom_theme_data");
	const JSON_Object * custom_theme_color_root = json_object_get_object(custom_theme_root, "colors");
	for (uint k = 0; k < ImGuiCol_COUNT; k++)
	{
		char index[5];
		itoa(k, index, 10);
		string number = index;
		string next_win = "color" + number;

		JSON_Array* color_array = json_object_get_array(custom_theme_color_root, next_win.c_str());
		json_array_replace_number(color_array, 0, custom_style.Colors[k].x);
		json_array_replace_number(color_array, 1, custom_style.Colors[k].y);
		json_array_replace_number(color_array, 2, custom_style.Colors[k].z);
		json_array_replace_number(color_array, 3, custom_style.Colors[k].w);

		number.clear();
		next_win.clear();
	}
}

void ModuleImgui::BlitUIConfigWindow()
{
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.5f, (App->window->GetHeight() - 23)), cond_flag);
	ImGui::SetNextWindowPos(ImVec2(App->window->GetWidth() * 0.5f, 23), cond_flag);
	ImGui::Begin("UI Config", &show_ui_conf_window, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
	
	if (ImGui::Checkbox("Dark Theme", &dark_theme))
	{
		if (dark_theme)
		{
			SetDarkTheme();
			light_theme = false;
			custom_theme = false;
		}
		else dark_theme = true;
	}

	ImGui::Separator();

	if (ImGui::Checkbox("White Theme", &light_theme))
	{
		if (light_theme)
		{
			SetLightTheme();
			dark_theme = false;
			custom_theme = false;
		}
		else light_theme = true;
	}

	ImGui::Separator();

	if (ImGui::Checkbox("Custom Theme", &custom_theme))
	{
		if (custom_theme)
		{
			SetCustomTheme();
			dark_theme = false;
			light_theme = false;
		}
		else custom_theme = true;
	}
	
	if (custom_theme)
	{
		//Save if custom theme has been edited
		bool changes = false;
		
		//All custom theme options
		ImGui::Text("Colors:"); ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)","RGBA colors of the all UI elements.");
		ImGui::ColorEdit4("C##0", *custom_style.Colors[ImGuiCol_Text].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Text", *custom_style.Colors[ImGuiCol_Text].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##1", *custom_style.Colors[ImGuiCol_TextDisabled].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Text Disabled", *custom_style.Colors[ImGuiCol_TextDisabled].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##2", *custom_style.Colors[ImGuiCol_WindowBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Window background", *custom_style.Colors[ImGuiCol_WindowBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##3", *custom_style.Colors[ImGuiCol_ChildWindowBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Child Window background", *custom_style.Colors[ImGuiCol_ChildWindowBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##4", *custom_style.Colors[ImGuiCol_PopupBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Popup background", *custom_style.Colors[ImGuiCol_PopupBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##5", *custom_style.Colors[ImGuiCol_Border].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Border", *custom_style.Colors[ImGuiCol_Border].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##6", *custom_style.Colors[ImGuiCol_BorderShadow].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Border Shadow", *custom_style.Colors[ImGuiCol_BorderShadow].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##7", *custom_style.Colors[ImGuiCol_FrameBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Frame background", *custom_style.Colors[ImGuiCol_FrameBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##8", *custom_style.Colors[ImGuiCol_FrameBgHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Frame Hovered background", *custom_style.Colors[ImGuiCol_FrameBgHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##9", *custom_style.Colors[ImGuiCol_FrameBgActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Frame Active background", *custom_style.Colors[ImGuiCol_FrameBgActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##10", *custom_style.Colors[ImGuiCol_TitleBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Title background", *custom_style.Colors[ImGuiCol_TitleBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##11", *custom_style.Colors[ImGuiCol_TitleBgCollapsed].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Title Collapsed background", *custom_style.Colors[ImGuiCol_TitleBgCollapsed].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##12", *custom_style.Colors[ImGuiCol_TitleBgActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Title Active background", *custom_style.Colors[ImGuiCol_TitleBgActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##13", *custom_style.Colors[ImGuiCol_MenuBarBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Menu Bar background", *custom_style.Colors[ImGuiCol_MenuBarBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##14", *custom_style.Colors[ImGuiCol_ScrollbarBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Scroll Bar background", *custom_style.Colors[ImGuiCol_ScrollbarBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##15", *custom_style.Colors[ImGuiCol_ScrollbarGrab].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Scroll Bar Grab", *custom_style.Colors[ImGuiCol_ScrollbarGrab].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##16", *custom_style.Colors[ImGuiCol_ScrollbarGrabHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Scroll Bar Grab Hovered", *custom_style.Colors[ImGuiCol_ScrollbarGrabHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##17", *custom_style.Colors[ImGuiCol_ScrollbarGrabActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Scroll Bar Grab Active", *custom_style.Colors[ImGuiCol_ScrollbarGrabActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##18", *custom_style.Colors[ImGuiCol_ComboBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Combo background", *custom_style.Colors[ImGuiCol_ComboBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##19", *custom_style.Colors[ImGuiCol_CheckMark].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Check Mark", *custom_style.Colors[ImGuiCol_CheckMark].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##20", *custom_style.Colors[ImGuiCol_SliderGrab].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Slider Grab", *custom_style.Colors[ImGuiCol_SliderGrab].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##21", *custom_style.Colors[ImGuiCol_SliderGrabActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("TeSlider Grab Active", *custom_style.Colors[ImGuiCol_SliderGrabActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##22", *custom_style.Colors[ImGuiCol_Button].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Button", *custom_style.Colors[ImGuiCol_Button].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##23", *custom_style.Colors[ImGuiCol_ButtonHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Button Hovered", *custom_style.Colors[ImGuiCol_ButtonHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##24", *custom_style.Colors[ImGuiCol_ButtonActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Button Active", *custom_style.Colors[ImGuiCol_ButtonActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##25", *custom_style.Colors[ImGuiCol_Header].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Header", *custom_style.Colors[ImGuiCol_Header].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##26", *custom_style.Colors[ImGuiCol_HeaderHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Header Hovered", *custom_style.Colors[ImGuiCol_HeaderHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##27", *custom_style.Colors[ImGuiCol_HeaderActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Header Active", *custom_style.Colors[ImGuiCol_HeaderActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##28", *custom_style.Colors[ImGuiCol_Separator].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Separator", *custom_style.Colors[ImGuiCol_Separator].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##29", *custom_style.Colors[ImGuiCol_SeparatorHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Separator Hovered", *custom_style.Colors[ImGuiCol_SeparatorHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##30", *custom_style.Colors[ImGuiCol_SeparatorActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Separator Active", *custom_style.Colors[ImGuiCol_SeparatorActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##31", *custom_style.Colors[ImGuiCol_ResizeGrip].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Resize Grip", *custom_style.Colors[ImGuiCol_ResizeGrip].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##32", *custom_style.Colors[ImGuiCol_ResizeGripHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Resize Grip Hovered", *custom_style.Colors[ImGuiCol_ResizeGripHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##33", *custom_style.Colors[ImGuiCol_ResizeGripActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Resize Grip Active", *custom_style.Colors[ImGuiCol_ResizeGripActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##34", *custom_style.Colors[ImGuiCol_CloseButton].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Close Button", *custom_style.Colors[ImGuiCol_CloseButton].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##35", *custom_style.Colors[ImGuiCol_CloseButtonHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Close Button Hovered", *custom_style.Colors[ImGuiCol_CloseButtonHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##36", *custom_style.Colors[ImGuiCol_CloseButtonActive].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Close Button Active", *custom_style.Colors[ImGuiCol_CloseButtonActive].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##37", *custom_style.Colors[ImGuiCol_PlotLines].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Plot Lines", *custom_style.Colors[ImGuiCol_PlotLines].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##38", *custom_style.Colors[ImGuiCol_PlotLinesHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Plot Lines Hovered", *custom_style.Colors[ImGuiCol_PlotLinesHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##39", *custom_style.Colors[ImGuiCol_PlotHistogram].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Plot Histogram", *custom_style.Colors[ImGuiCol_PlotHistogram].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##40", *custom_style.Colors[ImGuiCol_PlotHistogramHovered].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Plot Histogram Hovered", *custom_style.Colors[ImGuiCol_PlotHistogramHovered].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##41", *custom_style.Colors[ImGuiCol_TextSelectedBg].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Text Selected background", *custom_style.Colors[ImGuiCol_TextSelectedBg].Get_F_Array(), 0.0f, 1.0f))changes = true;
		ImGui::ColorEdit4("C##42", *custom_style.Colors[ImGuiCol_ModalWindowDarkening].Get_F_Array(), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel); ImGui::SameLine();
		if (ImGui::SliderFloat4("Modal Window Darkening", *custom_style.Colors[ImGuiCol_ModalWindowDarkening].Get_F_Array(), 0.0f, 1.0f))changes = true;
		
		//Upload imgui theme if custom theme has been edited
		if (changes)SetCustomTheme();
	}
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
	ImGui::Text("-DevIL: %i", ilGetInteger(IL_VERSION_NUM));

	ImGui::Text("\nWe don't own all this libraries and are not made by us");
	ImGui::Text("The engine is under MIT License");
	
	ImGui::Separator();
	ImGui::BulletText("Ferran Martin Vila");
	ImGui::SameLine();
	if (ImGui::Button("Info##f"))
	{
		ShellExecute(NULL, "open", "https://github.com/ferranmartinvila", NULL, NULL, SW_SHOWNORMAL);
	}
	ImGui::BulletText("Eric Sola Vila");
	ImGui::SameLine();
	if (ImGui::Button("Info##e"))
	{
		ShellExecute(NULL, "open", "https://github.com/HeladodePistacho", NULL, NULL, SW_SHOWNORMAL);
	}

	ImGui::End();
}

void ModuleImgui::BlitExitWindow()
{
	ImGui::SetNextWindowSize(ImVec2(App->window->GetWidth() * 0.5f, (App->window->GetHeight()) * 0.25f), cond_flag);
	ImGui::SetNextWindowPos(ImVec2(App->window->GetWidth() * 0.25f, App->window->GetHeight() * 0.15f), cond_flag);
	
	ImGui::Begin("Exit Window",&show_exit_window,ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
	
	ImGui::TextColored(ImVec4(0.5f, 0.4f, 0.2f, 1.0f), "-- Exit Window --");
	
	ImGui::Separator();
	ImGui::Separator();
	
	if (ImGui::Button("Exit", ImVec2(App->window->GetWidth() * 0.23f, App->window->GetHeight() * 0.05f)))
	{
		App->SetQuit();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(App->window->GetWidth() * 0.23f, App->window->GetHeight() * 0.05f)))
	{
		show_exit_window = false;
	}

	ImGui::Separator(); 
	ImGui::Separator();

	ImGui::End();
}

void ModuleImgui::SetDarkTheme()
{
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
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.40f);
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
	style->Colors[ImGuiCol_Header] = ImVec4(0.35f, 0.25f, 0.0f, 1.00f);
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
}

void ModuleImgui::SetLightTheme()
{
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

	style->Colors[ImGuiCol_Text] = ImVec4(1 - 0.93f,1 - 0.93f,1 - 0.93f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(1 - 0.24f, 1 - 0.23f, 1 - 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(1 - 0.05f, 1 - 0.05f, 1 - 0.05f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(1 - 0.07f, 1 - 0.07f, 1 - 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(1 - 0.05f, 1 - 0.05f, 1 - 0.05f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(1 - 0.80f, 1 - 0.80f, 1 - 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(1 - 0.92f, 1 - 0.91f, 1 - 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(1 - 0.24f, 1 - 0.23f, 1 - 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1 - 1.00f, 1 - 0.98f, 1 - 0.95f, 0.75f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(1 - 0.07f, 1 - 0.07f, 1 - 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(1 - 0.80f, 1 - 0.80f, 1 - 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(1 - 0.9f, 1 - 0.9f, 1 - 0.9f, 1.00f);
	style->Colors[ImGuiCol_ComboBg] = ImVec4(1 - 0.19f, 1 - 0.18f, 1 - 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(1 - 0.80f, 1 - 0.80f, 1 - 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(1 - 0.80f, 1 - 0.80f, 1 - 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(1 - 0.9f, 1 - 0.9f, 1 - 0.9f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(1 - 0.24f, 1 - 0.23f, 1 - 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(1 - 0.10f, 1 - 0.09f, 1 - 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(1 - 0.06f, 1 - 0.05f, 1 - 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(1 - 0.24f, 1 - 0.23f, 1 - 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(1 - 0.00f, 1 - 0.00f, 1 - 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(1 - 0.56f, 1 - 0.56f, 1 - 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(1 - 0.06f, 1 - 0.05f, 1 - 0.07f, 1.00f);
	style->Colors[ImGuiCol_CloseButton] = ImVec4(1 - 0.40f, 1 - 0.39f, 1 - 0.38f, 0.16f);
	style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(1 - 0.40f, 1 - 0.39f, 1 - 0.38f, 0.39f);
	style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(1 - 0.40f, 1 - 0.39f, 1 - 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(1 - 0.010f, 1 - 0.39f, 1 - 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1 - 0.25f, 1 - 1.00f, 1 - 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(1 - 0.90f, 1 - 0.80f, 1 - 0.30f, 1.0f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1 - 1.00f, 1 - 0.64f, 1 - 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(1 - 0.25f, 1 - 1.00f, 1 - 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1 - 1.00f, 1 - 0.98f, 1 - 0.95f, 0.73f);
}

void ModuleImgui::SetCustomTheme()
{
	ImGui::GetStyle() = custom_style;
}

// Get Methods ==================================
DockContext* ModuleImgui::GetWorkspace() const
{
	return father_dock;
}

ImGuiCond_ ModuleImgui::GetUICondFlag() const
{
	return cond_flag;
}

// Set Methods ==================================
void ModuleImgui::SetCondFlag(ImGuiCond_ cond)
{
	cond_flag = cond;
	cond_flag_timer.Start();
}

// Functionality ================================
void ModuleImgui::RenderUI()
{
	father_dock->EndWorkspace();
	ImGui::End();
	ImGui::Render();
}

void ModuleImgui::CallExitWindow()
{
	show_exit_window = !show_exit_window;
}

