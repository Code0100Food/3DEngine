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
#include "Scene.h"
#include "ModuleTextures.h"

#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "Engine/Glew/lib/Win32/glew32.lib")

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

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			LOG("[error] Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Model view Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

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

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	render_to_texture = new FrameTexture();
	render_to_texture->Create(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->position.x, App->camera->position.y, App->camera->position.z);

	for (uint i = 0; i < MAX_LIGHTS; ++i)
	{
		lights[i].Render();
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
	//Focus viewport texture
	render_to_texture->Bind();

	//Draw scene geometry
	EnableGLRenderFlags();
	App->geometry->Draw();
	
	//Focus render texture
	render_to_texture->UnBind();

	//Workspace window
	ImGui::SetNextWindowSize(ImVec2(render_to_texture->width, render_to_texture->height));
	ImGui::Begin("Render Workspace##window", 0, 0);

	render_dock->BeginWorkspace("Render Workspace");
	render_dock->BeginDock("Scene", 0, ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_::ImGuiWindowFlags_NoScrollWithMouse);

	ImGui::Image((void*)render_to_texture->texture_id, ImVec2(render_to_texture->width, render_to_texture->height), ImVec2(1, 1), ImVec2(0, 0));

	render_dock->EndDock();

	render_dock->EndWorkspace();
	ImGui::End();

	DisableGLRenderFlags();	

	// Rendering GUI
	App->imgui->RenderUI();

	SDL_GL_SwapWindow(App->window->window);

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	RELEASE(render_dock);
	RELEASE(render_to_texture);
	SDL_GL_DeleteContext(context);

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
}

// Functionality ================================
void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	if (render_to_texture)
	{
		render_to_texture->Destroy();
		render_to_texture->Create(width,height);
	}

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

bool ModuleRenderer3D::GetWireframe() const
{
	return wireframe;
}

bool ModuleRenderer3D::GetWireframeFront() const
{
	return front_wireframe;
}

void ModuleRenderer3D::DisableGLRenderFlags()
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_LIGHTING);
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

//------------------ FRAME TEXTURE ------------------\\

FrameTexture::FrameTexture() : frame_id(0), rbo_id(0), texture_id(0), width(0), height(0) {}

FrameTexture::~FrameTexture()
{
	Destroy();
}

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

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void FrameTexture::Bind()
{
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, frame_id);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/*glTranslatef(0.5, 0.5, 0.0);
	glRotatef(180, 0.0, 0.0, 1.0);
	glTranslatef(-0.5, -0.5, 0.0);*/
	
}

void FrameTexture::UnBind()
{
	glViewport(0, 0, App->window->GetWidth(), App->window->GetHeight());
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameTexture::Destroy()
{
	glDeleteFramebuffers(1, &frame_id);
	frame_id = 0;

	glDeleteTextures(1, &texture_id);
	texture_id = 0;

	glDeleteRenderbuffers(1, &rbo_id);
	rbo_id = 0;
}
