#ifndef _MODULE_RENDER_3D_H_
#define _MODULE_RENDER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "Light.h"
#include "MathGeoLib/MathGeoLib.h"

#define MAX_LIGHTS 8

class DockContext;
class ComponentCamera;

class FrameTexture
{
public:

	FrameTexture();
	~FrameTexture();

public:

	void Create(int, int);
	void Bind();
	void UnBind();
	void Destroy();

public:

	uint	frame_id = 0;
	uint	rbo_id = 0;
	uint	texture_id = 0;
	uint	depth_id = 0;

	int		width = 0;
	int		height = 0;

};


class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleRenderer3D();

public:

	bool			Awake(const JSON_Object* data_root)final;
	bool			Init() final;
	update_status	PreUpdate(float dt) final;
	update_status	Update(float dt)final;
	update_status	PostUpdate(float dt) final;
	bool			CleanUp() final;
	void			BlitConfigInfo()final;
	void			SaveConfigInfo(JSON_Object* data_root);

private:

	bool	vsync = false;
	bool	depth_test = false;
	bool	cull_face = false;
	bool	texture_2d = false;
	bool	dither = false;
	bool	wireframe = false;
	bool	front_wireframe = false;

	bool	lighting = false;
	float	lighting_color[4];

	bool	material_color = false;
	float	material_ambient[4];
	float	material_diffuse[4];

	bool	fog = false;
	float	fog_density = 0.0f;
	float	fog_color[4];

	bool	custom_clear = false;
	float	clear_color[4];
	float	clear_depth = 0.0f;
	float	min_render_distance = 0.0f;
	float	max_render_distance = 0.0f;

private:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;
	math::float3x3	NormalMatrix;
	math::float4x4	ModelMatrix, ViewMatrix, ProjectionMatrix;

	unsigned int	quadVAO = 0, quadVBO = 0;

	DockContext*	render_dock = nullptr;
	ImVec2			image_window_pos;

	//Editor Camera texture
	FrameTexture*	render_to_texture = nullptr;

	//Game Camera Texture
	FrameTexture*					game_to_texture = nullptr;
	std::vector<ComponentCamera*>	game_cameras;
	ComponentCamera*				main_camera = nullptr;

	bool			mouse_on_workspace = false;
	
	void CalculatePrespective(math::float4x4& target, float fovy, float aspect, float n, float f);

public:

	//Set Methods -----------
	void SetMinRenderDistance(float val);
	void SetMaxRenderDistance(float val);
	void SetMainCamera(ComponentCamera* new_main_cam);

	//Get Methods -----------
	bool	GetWireframe() const;
	bool	GetWireframeFront() const;
	bool	GetMouseOnWorkspace()const;
	float	GetMaxRenderDistance()const;
	ImVec2	GetSceneImagePos()const;

	//Functionality ---------
	void OnResize(int width, int height);
	void DisableGLRenderFlags();
	void EnableGLRenderFlags();

	void AddGameCamera(ComponentCamera* new_game_cam);
	void RemoveGameCamera(ComponentCamera* removed_game_cam);

private:

	//Functions to change the 
	void SetGameCameraView();
	void SetEditorCameraView();
	void CleanCameraView();
	void PrintPlayPauseButton() const;

	
};
#endif // !_MODULE_RENDER_3D_H_


