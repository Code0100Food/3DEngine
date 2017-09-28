#ifndef _MODULE_RENDER_3D_H_
#define _MODULE_RENDER_3D_H_

#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:

	ModuleRenderer3D(const char* _name, MODULE_ID _id, bool _config_menu, bool _enabled = true);
	~ModuleRenderer3D();

public:

	bool			Awake(const JSON_Object* data_root)final;
	bool			Init() final;
	bool			Start()final;
	update_status	PreUpdate(float dt) final;
	update_status	Update(float dt)final;
	update_status	PostUpdate(float dt) final;
	bool			CleanUp() final;
	void			BlitConfigInfo()final;
	void			SaveConfigInfo(JSON_Object* data_root);

public:

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

public:

	Light			lights[MAX_LIGHTS];
	SDL_GLContext	context;
	mat3x3			NormalMatrix;
	mat4x4			ModelMatrix, ViewMatrix, ProjectionMatrix;

public:

	void OnResize(int width, int height);
	bool GetWireframe() const;
	bool GetWireframeFront() const;
	void DisableGLRenderFlags();
	void EnableGLRenderFlags();

};


#endif // !_MODULE_RENDER_3D_H_


