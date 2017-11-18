#include "Globals.h"
#include "Application.h"
#include "GeometryManager.h"
#include "ModuleRenderer3D.h"
#include "Glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "ModuleWindow.h"
#include "ModuleCamera3D.h"
#include "FileSystem.h"
#include "ModuleAudio.h"
#include "ModuleImgui.h"
#include "imgui/imgui_dock.h"
#include "ModuleTextures.h"
#include "ModuleScene.h"
#include "ModuleInput.h"

#ifdef _DEBUG
#pragma comment (lib, "Engine/imgui/lib/libgizmo.lib")
#else
#pragma comment (lib, "Engine/imgui/lib/libgizmoRelease.lib")
#endif // DEBUG

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Engine/Glew/lib/Win32/glew32.lib")

// Frame Texture --------------------------------
// Constructors =================================
FrameTexture::FrameTexture() : frame_id(0), rbo_id(0), texture_id(0), width(0), height(0)
{

}

// Destructors ==================================
FrameTexture::~FrameTexture()
{
	Destroy();
}

// Functionality ================================
void FrameTexture::Create(int width, int height)
{
	//Create The Frame Buffer
	glGenFramebuffers(1, &frame_id);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);

	//Generate the Texture
	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	this->width = width;
	this->height = height;

	glGenTextures(1, &depth_id);
	glBindTexture(GL_TEXTURE_2D, depth_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	/*unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);*/


	//Attach texture to the frame buffer
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_id, 0);
	//glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_id, 0);//optional


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		LOG("Frame Buffer Load nice");
	}
	else
	{
		LOG("[error] Framebuffer error")
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FrameTexture::Bind()
{
	ImVec2 window_size;
	
	if (this == App->renderer3D->GetFrameTextureRender())
	{
		window_size = App->imgui->GetWorkspace()->GetDockbyLabel("Scene##texture")->size;

		if (window_size.x != width || window_size.y != height)
		{
			App->renderer3D->OnSceneResize(window_size.x, window_size.y);
		}
	
	}

	if (this == App->renderer3D->GetFrameTextureGame())
	{
		window_size = App->imgui->GetWorkspace()->GetDockbyLabel("Game##texture")->size;

		if (window_size.x != width || window_size.y != height)
		{
			App->renderer3D->OnGameResize(window_size.x, window_size.y);
		}

	}

	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void FrameTexture::UnBind()
{
	//glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameTexture::Destroy()
{
	glDeleteFramebuffers(1, &frame_id);
	frame_id = 0;

	glDeleteTextures(1, &texture_id);
	texture_id = 0;

	glDeleteTextures(1, &depth_id);
	depth_id = 0;
}

// Module Renderer3D ----------------------------
// Constructors =================================
ModuleRenderer3D::ModuleRenderer3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled) : Module(_name, _id, _config_menu, _enabled)
{

}

// Destructors ==================================
ModuleRenderer3D::~ModuleRenderer3D()
{

}

// Game Loop ====================================
bool ModuleRenderer3D::Awake(const JSON_Object * data_root)
{
	vsync = json_object_get_boolean(data_root, "vsync");
	
	depth_test = json_object_get_boolean(data_root, "depth_test");
	
	cull_face = json_object_get_boolean(data_root, "cull_face");
	
	texture_2d = json_object_get_boolean(data_root, "texture_2d");
	
	dither = json_object_get_boolean(data_root, "dither");

	wireframe = json_object_get_boolean(data_root, "wireframe");

	front_wireframe = json_object_get_boolean(data_root, "front_wireframe");

	lighting = json_object_get_boolean(data_root, "lighting");
	JSON_Array* lighting_color_array = json_object_get_array(data_root, "lighting_color");
	lighting_color[0] = json_array_get_number(lighting_color_array, 0);
	lighting_color[1] = json_array_get_number(lighting_color_array, 1);
	lighting_color[2] = json_array_get_number(lighting_color_array, 2);
	lighting_color[3] = json_array_get_number(lighting_color_array, 3);
	
	material_color = json_object_get_boolean(data_root, "material_color");
	JSON_Array* material_ambient_array = json_object_get_array(data_root, "material_ambient");
	material_ambient[0] = json_array_get_number(material_ambient_array, 0);
	material_ambient[1] = json_array_get_number(material_ambient_array, 1);
	material_ambient[2] = json_array_get_number(material_ambient_array, 2);
	material_ambient[3] = json_array_get_number(material_ambient_array, 3);
	JSON_Array* material_diffuse_array = json_object_get_array(data_root, "material_diffuse");
	material_diffuse[0] = json_array_get_number(material_diffuse_array, 0);
	material_diffuse[1] = json_array_get_number(material_diffuse_array, 1);
	material_diffuse[2] = json_array_get_number(material_diffuse_array, 2);
	material_diffuse[3] = json_array_get_number(material_diffuse_array, 3);
	
	fog = json_object_get_boolean(data_root, "fog");
	fog_density = json_object_get_number(data_root, "fog_density");
	JSON_Array* fog_color_array = json_object_get_array(data_root, "fog_color");
	fog_color[0] = json_array_get_number(fog_color_array, 0);
	fog_color[1] = json_array_get_number(fog_color_array, 1);
	fog_color[2] = json_array_get_number(fog_color_array, 2);
	fog_color[3] = json_array_get_number(fog_color_array, 3);

	custom_clear = json_object_get_boolean(data_root, "custom_clear");
	JSON_Array* clear_color_array = json_object_get_array(data_root, "clear_color");
	clear_color[0] = json_array_get_number(clear_color_array, 0);
	clear_color[1] = json_array_get_number(clear_color_array, 1);
	clear_color[2] = json_array_get_number(clear_color_array, 2);
	clear_color[3] = json_array_get_number(clear_color_array, 3);
	
	clear_depth = json_object_get_number(data_root, "clear_depth");
	
//	min_render_distance = json_object_get_number(data_root, "min_render_distance");
	
	//max_render_distance = json_object_get_number(data_root, "max_render_distance");

	config_menu = true;

	return true;
}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	render_dock = new DockContext();
	
	//OPENGL initialization
	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		LOG("[error] OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}

	GLenum glew_error = glewInit();
	if (glew_error != GL_NO_ERROR)
	{
		LOG("[error]Error initializing GLew! %s\n", glewGetErrorString(glew_error));
	}
	else
	{
		LOG("Using GLew: %s", glewGetString(GLEW_VERSION));
	}

	if(ret == true)
	{
		//Use Vsync
		if (vsync && SDL_GL_SetSwapInterval(1) < 0)
		{
			LOG("[error] Unable to set VSync! SDL Error: %s\n", SDL_GetError());
		}
		else if(!vsync)
		{
			SDL_GL_SetSwapInterval(0);
		}

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
				
		//Initialize clear color
		if (custom_clear)
		{
			glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
			glClearDepth(clear_depth);
		}
		else
		{
			glClearColor(0.0, 0.0, 0.0, 0.0);
			glClearDepth(1.0f);
		}

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Enable OpenGL Options
		if (depth_test)
		{
			glEnable(GL_DEPTH_TEST);
		}

		if (cull_face)
		{
			glEnable(GL_CULL_FACE);
		}

		if (texture_2d)
		{
			glEnable(GL_TEXTURE_2D);
		}

		if (dither)
		{
			glEnable(GL_DITHER);
		}

		//Enable lines antialiasing
		glEnable(GL_LINE_SMOOTH);

		//Initialize lighting states
		if (lighting)
		{
			glEnable(GL_LIGHTING);
			GLfloat l_color[] = { lighting_color[0], lighting_color[1], lighting_color[2], lighting_color[3] };
			glLightModelfv(GL_EMISSION, l_color);
		}

		//Initialize material states
		if (material_color)
		{
			glEnable(GL_COLOR_MATERIAL);

			GLfloat m_ambient[] = { material_ambient[0], material_ambient[1], material_ambient[2], material_ambient[3] };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient);
			GLfloat m_diffuse[] = { material_diffuse[0], material_diffuse[1], material_diffuse[2], material_diffuse[3] };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse);
		}

		//Initialize fog states
		if (fog)
		{
			glEnable(GL_FOG);

			const GLfloat f_color[4] = { fog_color[0], fog_color[1], fog_color[2], fog_color[3] };
			glFogfv(GL_FOG_COLOR, f_color);
			glFogf(GL_FOG_DENSITY, fog_density);
		}

		//Initialize default light
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
			
		lights[0].Active(true);
	
	}

	//gizmos
	trans_gizmo = CreateMoveGizmo();
	rotate_gizmo = CreateRotateGizmo();
	scale_gizmo = CreateScaleGizmo();

	print_gizmo = trans_gizmo;

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	render_to_texture = new FrameTexture();
	render_to_texture->Create(App->window->GetWidth(), App->window->GetHeight());

	game_to_texture = new FrameTexture();
	game_to_texture->Create(App->window->GetWidth(), App->window->GetHeight());

	return ret;
}

bool ModuleRenderer3D::Start()
{
	return true;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	//Clean the buffers 
	CleanCameraView();

	//Set the editor camera [Projection and view matrix]
	SetEditorCameraView();
	
	// light 0 on cam pos
	lights[0].SetPos(App->camera->GetPosition().x, App->camera->GetPosition().y, App->camera->GetPosition().z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
	{
		lights[i].Render();
	}

	if (print_gizmo)
	{
		print_gizmo->SetCameraMatrix(camera_view_matrix.ptr(), App->camera->editor_camera_frustrum.ProjectionMatrix().Transposed().ptr());
	}

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	//Info Source
	//http://www.pascalgamedevelopment.com/showthread.php?6617-drawing-3d-geometrical-shapes-using-opengl-but-without-glu

	
	
	return update_status::UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	//EDITOR CAMERA VIEW
	//Focus viewport texture
	render_to_texture->Bind();

	//Draw scene geometry
	EnableGLRenderFlags();
	App->geometry->Draw();
	
	//Draw / update scene objects
	App->scene->SceneUpdate(dt);

	glBegin(GL_LINES);
	glLineWidth(3.0f);
	glVertex3f(App->camera->mouse_picking.a.x, App->camera->mouse_picking.a.y, App->camera->mouse_picking.a.z); glVertex3f(App->camera->mouse_picking.b.x, App->camera->mouse_picking.b.y, App->camera->mouse_picking.b.z);
	glLineWidth(1.0f);
	glEnd();

	//Focus render texture
	render_to_texture->UnBind();

	if (main_camera)
	{
		SetGameCameraView();
		game_to_texture->Bind();
		App->geometry->Draw();
		App->scene->SceneUpdate(dt);
		float lol2[4] = { 1.0f, 1.0f, 0.5f,  1.0f };
		App->camera->editor_camera_frustrum.Draw(3.0f, lol2);
		glBegin(GL_LINES);
		glLineWidth(3.0f);
		glVertex3f(App->camera->mouse_picking.a.x, App->camera->mouse_picking.a.y, App->camera->mouse_picking.a.z); glVertex3f(App->camera->mouse_picking.b.x, App->camera->mouse_picking.b.y, App->camera->mouse_picking.b.z);
		glLineWidth(1.0f);
		glEnd();

		game_to_texture->UnBind();
		CleanCameraView();
	//	SetEditorCameraView();
	}

	//Scene and Game Dock
	App->imgui->GetWorkspace()->BeginDock("Game##texture", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);
	ImGui::Image((void*)game_to_texture->texture_id, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	game_window_size = ImGui::GetItemRectSize();
	App->imgui->GetWorkspace()->EndDock();

	App->imgui->GetWorkspace()->BeginDock("Scene##texture", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);

	//Detect if the mouse is inside the workspace
	mouse_on_workspace = ImGui::IsMouseHoveringWindow();

	ImGui::Image((void*)render_to_texture->texture_id, ImGui::GetContentRegionAvail(), ImVec2(0, 1), ImVec2(1, 0));
	image_window_pos = ImGui::GetItemRectMin();
	image_window_size = ImGui::GetItemRectSize();
	App->imgui->GetWorkspace()->EndDock();
	

	DisableGLRenderFlags();	

	if (print_gizmo)
	{
		print_gizmo->OnMouseMove((App->input->GetMouseX() - (image_window_pos.x - SCENE_BORDER_X)), (App->input->GetMouseY() - (image_window_pos.y - SCENE_BORDER_Y)));


		if (App->scene->GetSelectedGameObject() && App->camera->GetGizmoClicked())
		{
			ComponentTransform* tmp = (ComponentTransform*)App->scene->GetSelectedGameObject()->FindComponent(COMPONENT_TYPE::COMP_TRANSFORMATION);
			tmp->UpdateRotationPositionScale();
		}

	}

	// Rendering GUI
	App->imgui->RenderUI();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	RELEASE(trans_gizmo);
	RELEASE(rotate_gizmo);
	RELEASE(scale_gizmo);

	RELEASE(render_dock);
	RELEASE(render_to_texture);
	RELEASE(game_to_texture);
	SDL_GL_DeleteContext(context);

	game_cameras.clear();

	return true;
}

void ModuleRenderer3D::BlitConfigInfo()
{
	ImGui::Text("GLew Version: ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.64f, 0.0f, 1.0f), "%s", glewGetString(GLEW_VERSION));
	ImGui::Separator();

	//Vsync check box
	if (ImGui::Checkbox("VSync", &vsync))
	{
		if(vsync)
		{
			if (SDL_GL_SetSwapInterval(1) < 0)
			{
				LOG("[error] Unable to set VSync! SDL Error: %s\n", SDL_GetError());
			}
		}
		else SDL_GL_SetSwapInterval(0);
		App->audio->PlayFxForInput(CHECKBOX_FX);
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF VSync.");
	
	//Depth Test check box
	if (ImGui::Checkbox("Depht Test", &depth_test))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);

		if (depth_test)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF the depth pixel painting.");

	//Cull Test check box
	if (ImGui::Checkbox("Cull Test", &cull_face))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (cull_face)
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF the triangle culling.");

	//Texture 2D check box
	if (ImGui::Checkbox("Texture 2D", &texture_2d))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (texture_2d)
		{
			glEnable(GL_TEXTURE_2D);
		}
		else
		{
			glDisable(GL_TEXTURE_2D);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Allows to texture in 2D");

	//Dither check box
	if (ImGui::Checkbox("Dither", &dither))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (dither)
		{
			glEnable(GL_DITHER);
		}
		else
		{
			glDisable(GL_DITHER);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Dither color components before they are written to the color buffer.");
	ImGui::Separator();


	//Lighting data ---------
	if (ImGui::Checkbox("Lighting", &lighting))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (lighting)
		{
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHTING);
		}

	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turn ON/OFF the lights");
	if (lighting)
	{
		if (ImGui::DragFloat4("Lighting Color", lighting_color, 0.05, 0.0, 1.0, "%.2f"))
		{
			GLfloat LightModelAmbient[] = { lighting_color[0], lighting_color[1], lighting_color[2],lighting_color[3] };
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		}
	}
	// ----------------------

	ImGui::Separator();
	
	//Material Data ---------
	if (ImGui::Checkbox("Color Material", &material_color))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (material_color)
		{
			glEnable(GL_COLOR_MATERIAL);
		}
		else
		{
			glDisable(GL_COLOR_MATERIAL);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Allows to have one or more material parameters tracking the current color");
	if (material_color)
	{
		if (ImGui::DragFloat4("Material Ambient", material_ambient, 0.05, 0.0, 1.0, "%.2f"))
		{
			GLfloat m_ambient[] = { material_ambient[0], material_ambient[1], material_ambient[2], material_ambient[3] };
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_ambient);
		}
		if (ImGui::DragFloat4("Material Diffuse", material_diffuse, 0.05, 0.0, 1.0, "%.2f"))
		{
			GLfloat m_diffuse[] = { material_diffuse[0], material_diffuse[1], material_diffuse[2], material_diffuse[3] };
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_diffuse);
		}
	}
	// ----------------------

	ImGui::Separator();

	//Fog Data --------------
	if (ImGui::Checkbox("Fog", &fog))
	{
		App->audio->PlayFxForInput(CHECKBOX_FX);
		if (fog)
		{
			glEnable(GL_FOG);
			glFogf(GL_FOG_DENSITY, fog_density);
		}
		else
		{
			glDisable(GL_FOG);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turns ON/OFF the Fog");

	if (fog)
	{
		if (ImGui::DragFloat4("Fog Color", fog_color, 0.05, 0.0, 1.0, "%.2f"))
		{
			GLfloat color[] = { fog_color[0], fog_color[1], fog_color[2], fog_color[3] };
			glFogfv(GL_FOG_COLOR, color);
		}
		if (ImGui::SliderFloat("Density", &fog_density, 0.0f, 1.0f))
		{
			App->audio->PlayFxForInput(SLICE_TICK_FX);
			glFogf(GL_FOG_DENSITY, fog_density);
		}
	}
	// ----------------------

	ImGui::Separator();

	//Wireframe -------------

	ImGui::Checkbox("WireFrame", &wireframe);
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Turns ON/OFF the WireFrame mode");

	if (wireframe)
	{
		ImGui::Checkbox("Front Mode", &front_wireframe);
	}
	//-----------------------

	ImGui::Separator();

	//Clear Data ------------
	if(ImGui::Checkbox("Custom Clear", &custom_clear))
	{
		App->audio->PlayFxForInput(SLICE_TICK_FX);
		if (!custom_clear)
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClearDepth(1.0f);
		}
		else
		{
			glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
			glClearDepth(clear_depth);
		}
	}
	ImGui::SameLine(); ImGui::MyShowHelpMarker("(?)", "Specify the red, green, blue, and alpha values used when the color buffers are cleared");
	if (custom_clear)
	{
		if (ImGui::DragFloat4("Clear Color", clear_color, 0.05, 0.0, 1.0, "%.2f"))
		{
			glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
		}
		if (ImGui::DragFloat("Clear Depth", &clear_depth, 0.05, 0.0, 1.0, "%.2f"))
		{
			glClearDepth(clear_depth);
		}
	}
	// ----------------------
}

void ModuleRenderer3D::SaveConfigInfo(JSON_Object * data_root)
{
	json_object_set_boolean(data_root, "vsync", vsync);
	json_object_set_boolean(data_root, "depth_test", depth_test);
	json_object_set_boolean(data_root, "cull_face", cull_face);
	json_object_set_boolean(data_root, "texture_2d", texture_2d);
	json_object_set_boolean(data_root, "dither", dither);
	// Lighting data
	json_object_set_boolean(data_root, "lighting", lighting);
	json_array_t*_array = json_object_get_array(data_root, "lighting_color");
	json_array_replace_number(_array, 0, lighting_color[0]);
	json_array_replace_number(_array, 1, lighting_color[1]);
	json_array_replace_number(_array, 2, lighting_color[2]);
	json_array_replace_number(_array, 3, lighting_color[3]);
	// Material data
	json_object_set_boolean(data_root, "material_color", material_color);
	_array = json_object_get_array(data_root, "material_ambient");
	json_array_replace_number(_array, 0, material_ambient[0]);
	json_array_replace_number(_array, 1, material_ambient[1]);
	json_array_replace_number(_array, 2, material_ambient[2]);
	json_array_replace_number(_array, 3, material_ambient[3]);
	_array = json_object_get_array(data_root, "material_diffuse");
	json_array_replace_number(_array, 0, material_diffuse[0]);
	json_array_replace_number(_array, 1, material_diffuse[1]);
	json_array_replace_number(_array, 2, material_diffuse[2]);
	json_array_replace_number(_array, 3, material_diffuse[3]);
	//Wireframe data
	json_object_set_boolean(data_root, "wireframe", wireframe);
	json_object_set_boolean(data_root, "front_wireframe", front_wireframe);

	//Fog data
	json_object_set_boolean(data_root, "fog", fog);
	json_object_set_number(data_root, "fog_density", fog_density);
	_array = json_object_get_array(data_root, "fog_color");
	json_array_replace_number(_array, 0, fog_color[0]);
	json_array_replace_number(_array, 1, fog_color[1]);
	json_array_replace_number(_array, 2, fog_color[2]);
	json_array_replace_number(_array, 3, fog_color[3]);
	//Clear data
	json_object_set_boolean(data_root, "custom_clear", custom_clear);
	_array = json_object_get_array(data_root, "clear_color");
	json_array_replace_number(_array, 0, clear_color[0]);
	json_array_replace_number(_array, 1, clear_color[1]);
	json_array_replace_number(_array, 2, clear_color[2]);
	json_array_replace_number(_array, 3, clear_color[3]);

	json_object_set_number(data_root, "clear_depth", clear_depth);

	json_object_set_number(data_root, "min_render_distance", min_render_distance);

	json_object_set_number(data_root, "max_render_distance", max_render_distance);
}

void ModuleRenderer3D::CalculatePrespective(math::float4x4& target, float fovy, float aspect, float n, float f)
{
	float coty = 1.0f / tan(fovy * (float)HAVE_M_PI / 360.0f);

	target.ptr()[0] = coty / aspect;
	target.ptr()[5] = coty;
	target.ptr()[10] = (n + f) / (n - f);
	target.ptr()[11] = -1.0f;
	target.ptr()[14] = 2.0f * n * f / (n - f);
	target.ptr()[15] = 0.0f;
}

// Set Methods ==================================

void ModuleRenderer3D::SetMainCamera(ComponentCamera* new_main_cam)
{

	for (std::vector<ComponentCamera*>::iterator item = game_cameras.begin(); item != game_cameras.end(); item++)
	{
		if ((*item) == new_main_cam)
		{
			main_camera = new_main_cam;
			continue;
		}
		else
		{
			(*item)->SetIsMain(false);
		}
	}


}

void ModuleRenderer3D::SetGizmo(GUIZMO_STATE state)
{
	switch (state)
	{
	case GUIZMO_NONE:
		print_gizmo = nullptr;
		break;
	case GUIZMO_TRANSLATE:
		print_gizmo = trans_gizmo;
		break;
	case GUIZMO_ROTATE:
		print_gizmo = rotate_gizmo;
		break;
	case GUIZMO_SCALE:
		print_gizmo = scale_gizmo;
		break;
	}
}

// Get Methods ==================================
bool ModuleRenderer3D::GetWireframe() const
{
	return wireframe;
}

bool ModuleRenderer3D::GetWireframeFront() const
{
	return front_wireframe;
}

bool ModuleRenderer3D::GetMouseOnWorkspace()const
{
	return mouse_on_workspace;
}

float ModuleRenderer3D::GetMaxRenderDistance() const
{
	return max_render_distance;
}

ImVec2 ModuleRenderer3D::GetSceneImagePos() const
{
	return image_window_pos;
}

ImVec2 ModuleRenderer3D::GetSceneImageSize() const
{
	return image_window_size;
}

ImVec2 ModuleRenderer3D::GetGameImageSize() const
{
	return game_window_size;
}

const FrameTexture * ModuleRenderer3D::GetFrameTextureRender() const
{
	return render_to_texture;
}

const FrameTexture * ModuleRenderer3D::GetFrameTextureGame() const
{
	return game_to_texture;
}

IGizmo* ModuleRenderer3D::GetGizmo() const
{
	return print_gizmo;
}

// Functionality ================================
void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	/*glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->GetProjectionMatrixTransposed());*/

	if (render_to_texture)
	{
		render_to_texture->Destroy();
		render_to_texture->Create(width,height);
	}

	if (game_to_texture)
	{
		game_to_texture->Destroy();
		game_to_texture->Create(width, height);
	}

	App->window->SetAspectRatio(width, height);
	App->camera->SetVerticalFovRad(App->camera->editor_camera_frustrum.verticalFov, App->window->GetAspectRatio());
	
	trans_gizmo->SetScreenDimension(width, height);
	rotate_gizmo->SetScreenDimension(width, height);
	scale_gizmo->SetScreenDimension(width, height);
	
	for (std::vector<ComponentCamera*>::iterator item = game_cameras.begin(); item != game_cameras.end(); item++)
	{
		(*item)->SetVerticalFov((*item)->GetFrustum().verticalFov * RADTODEG, App->window->GetAspectRatio());
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::OnSceneResize(int width, int height)
{
	render_to_texture->Destroy();
	render_to_texture->Create(width, height);
	
	App->window->SetSceneAspectRatio(width, height);
	App->camera->SetVerticalFovRad(App->camera->editor_camera_frustrum.verticalFov, App->window->GetSceneAspectRation());

	trans_gizmo->SetScreenDimension(width, height);
	rotate_gizmo->SetScreenDimension(width, height);
	scale_gizmo->SetScreenDimension(width, height);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera_view_matrix.ptr());
}

void ModuleRenderer3D::OnGameResize(int width, int height)
{
	game_to_texture->Destroy();
	game_to_texture->Create(width, height);

	App->window->SetGameAspectRatio(width, height);
	
	for (std::vector<ComponentCamera*>::iterator item = game_cameras.begin(); item != game_cameras.end(); item++)
	{
		(*item)->SetVerticalFov((*item)->GetFrustum().verticalFov * RADTODEG, App->window->GetGameAspectRatio());
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::DisableGLRenderFlags()
{
	if (wireframe)glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (lighting)glDisable(GL_LIGHTING);
}

void ModuleRenderer3D::EnableGLRenderFlags()
{
	if (lighting)glEnable(GL_LIGHTING);
	if (wireframe)
	{
		if (front_wireframe)
		{
			glPolygonMode(GL_FRONT, GL_LINE);
		}
		else
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
	}
}

void ModuleRenderer3D::AddGameCamera(ComponentCamera * new_game_cam)
{
	//Look if vector is empty
	if (game_cameras.empty())
	{
		game_cameras.push_back(new_game_cam);
		main_camera = new_game_cam;
		return;
	}

	//Look if camera is already into game_cameras
	bool finded = false;
	for (std::vector<ComponentCamera*>::iterator item = game_cameras.begin(); item != game_cameras.end(); item++)
	{
		if ((*item) == new_game_cam)
		{
			return;
		}
	}

	if (!finded)
	{
		game_cameras.push_back(new_game_cam);
	}

}

void ModuleRenderer3D::RemoveGameCamera(ComponentCamera * removed_game_cam)
{
	for (std::vector<ComponentCamera*>::iterator item = game_cameras.begin(); item != game_cameras.end(); item++)
	{
		if ((*item) == removed_game_cam)
		{
			item = game_cameras.erase(item);
			break;
		}
	}
}

void ModuleRenderer3D::SetGameCameraView()
{
	//Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(main_camera->GetFrustum().ProjectionMatrix().Transposed().ptr());

	//Set view matrix as a 4x4 matrix
	game_view_matrix = main_camera->GetFrustum().ViewMatrix();
	game_view_matrix.Transpose();

	//Set the view matrix to the render
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(game_view_matrix.ptr());

	lights[0].SetPos(main_camera->GetFrustum().pos.x, main_camera->GetFrustum().pos.y, main_camera->GetFrustum().pos.z);
	lights[0].Render();
}

void ModuleRenderer3D::SetEditorCameraView()
{
	//Set projection matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(App->camera->editor_camera_frustrum.ProjectionMatrix().Transposed().ptr());

	//Set view matrix as a 4x4 matrix
	camera_view_matrix = App->camera->editor_camera_frustrum.ViewMatrix();
	camera_view_matrix.Transpose();

	//Set the view matrix to the render
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(camera_view_matrix.ptr());
}

void ModuleRenderer3D::CleanCameraView()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
}

void ModuleRenderer3D::PrintPlayPauseButton() const
{
	//Transform
	ImGui::Image((ImTextureID)App->textures->transform_icon, ImVec2(23, 23));

	ImGui::SameLine();
	if (ImGui::Button("Local##button"))
	{
		trans_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_LOCAL);
		rotate_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_LOCAL);
		scale_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_LOCAL);

	}
	ImGui::SameLine();
	if (ImGui::Button("Global##button"))
	{
		trans_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_WORLD);
		rotate_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_WORLD);
		scale_gizmo->SetLocation(IGizmo::LOCATION::LOCATE_WORLD);
	}

	ImGui::SameLine();

	//Plat Pause Next Button
	ImGui::SameLine(ImGui::GetWindowContentRegionWidth() / 2);
	if (ImGui::ImageButton((ImTextureID)App->textures->pause_icon_id, ImVec2(15, 15), ImVec2(0, 0), ImVec2(1, 1)))
	{
		App->scene->PauseGame();
	}

	ImGui::SameLine((ImGui::GetWindowContentRegionWidth() / 2) - 35);
	if (ImGui::ImageButton((ImTextureID)App->textures->play_icon_id, ImVec2(15, 15)))
	{
		if (App->scene->GetSceneState() == SCENE_UPDATE_STATE::EDIT_SCENE_STATE)
		{
			if (App->imgui->GetWorkspace()->GetDockbyLabel("Game##texture")->active == false)
			{
				App->imgui->GetWorkspace()->GetDockbyLabel("Scene##texture")->active = false;
				App->imgui->GetWorkspace()->GetDockbyLabel("Game##texture")->active = true;
			}	
		}
		if (App->scene->GetSceneState() == SCENE_UPDATE_STATE::PLAY_SCENE_STATE)
		{
			if (App->imgui->GetWorkspace()->GetDockbyLabel("Scene##texture")->active == false)
			{
				App->imgui->GetWorkspace()->GetDockbyLabel("Scene##texture")->active = true;
				App->imgui->GetWorkspace()->GetDockbyLabel("Game##texture")->active = false;
			}
		
		}

		App->scene->PlayGame();
	}
	
	ImGui::SameLine((ImGui::GetWindowContentRegionWidth() / 2) + 35);
	if (ImGui::ImageButton((ImTextureID)App->textures->next_icon_id, ImVec2(15, 15)))
	{
		App->scene->NextGameFrame();
	}
}
